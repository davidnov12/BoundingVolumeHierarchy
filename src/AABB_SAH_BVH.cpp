/*
* RayTracing pro GPUEngine
* Diplomova prace - master's thesis
* Bc. David Novák
* FIT VUT Brno
* 2018/2019
*
* AABB_SAH_BVH.cpp
*/

#include <AABB_SAH_BVH.h>

using BVHNode = ge::sg::BVH_Node<ge::sg::AABB>;


void ge::sg::AABB_SAH_BVH::build() {

	computeCenters(_firstPrimitive, _lastPrimitive);
	
	ge::sg::AABB bvol;
	rootNode = std::make_shared<BVHNode>(bvol, _firstPrimitive, _lastPrimitive + (0));

	recursiveBuild(*rootNode, _firstPrimitive, maxDepth - 1, DivideAxis::X_AXIS);

#ifdef CPU_BVH_MEASURE
	std::cout << "Bound boxes " << minBB * 1000.0f << "ms" << std::endl;
	std::cout << "Sort " << sort * 1000.0f << "ms" << std::endl;
	std::cout << "Division " << divide * 1000.0f << "ms" << std::endl;
#endif

}

void ge::sg::AABB_SAH_BVH::setSplitPartitions(unsigned numberOfParts) {
	nrOfPartitions = numberOfParts;
}

std::shared_ptr<BVHNode> ge::sg::AABB_SAH_BVH::getRoot() {
	return rootNode;
}
int idx = 0;
void ge::sg::AABB_SAH_BVH::recursiveBuild(BVHNode & node, ge::sg::IndexedTriangleIterator & start, unsigned currentDepth, DivideAxis axis) {

	// Bounding Volume refit
	glm::vec3 _min(std::numeric_limits<float>::max()), _max(-std::numeric_limits<float>::max());

	assert((node.last - node.first) > 0);
	
#ifdef CPU_BVH_MEASURE
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
#endif
	
	for (auto i = node.first; i < node.last; i = i + 1) {
		
		assert(node.first->v0 != nullptr);
		assert(node.last->v0 != nullptr);

		_min.x = std::min(_min.x, std::min(i->v0[0], std::min(i->v1[0], i->v2[0])));
		_min.y = std::min(_min.y, std::min(i->v0[1], std::min(i->v1[1], i->v2[1])));
		_min.z = std::min(_min.z, std::min(i->v0[2], std::min(i->v1[2], i->v2[2])));

		_max.x = std::max(_max.x, std::max(i->v0[0], std::max(i->v1[0], i->v2[0])));
		_max.y = std::max(_max.y, std::max(i->v0[1], std::max(i->v1[1], i->v2[1])));
		_max.z = std::max(_max.z, std::max(i->v0[2], std::max(i->v1[2], i->v2[2])));

		
	}
	
	glm::vec3 dir = _max - _min;

	node.volume.min = _min;
	node.volume.max = _max;

	assert(abs(_min.x - std::numeric_limits<float>::max()) > 1e-5);

#ifdef CPU_BVH_MEASURE
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	minBB += time_span.count();
#endif

	
	
	node.left = nullptr, node.right = nullptr;
	
	// Recursion end - max depth reached
	if (currentDepth == 0) {
		return;
	}

	if ((node.last - node.first) < minVolumePrimitives)
		return;

	float minCoord, maxCoord;
	minCoord = axis == DivideAxis::X_AXIS ? _min.x : axis == DivideAxis::Y_AXIS ? _min.y : _min.z;
	maxCoord = axis == DivideAxis::X_AXIS ? _max.x : axis == DivideAxis::Y_AXIS ? _max.y : _max.z;
	
	if (maxCoord - minCoord <= 0.0f)
		return;

#ifdef CPU_BVH_MEASURE
	t1 = std::chrono::high_resolution_clock::now();
#endif

	// Sort centers by one axis
	if (node.first.getIndices() == nullptr)
		sortCenters(node.first, node.last, start, axis);
	else
		sortCentersIndexed(node.first, node.last, start, axis);

#ifdef CPU_BVH_MEASURE
	t2 = std::chrono::high_resolution_clock::now();
	time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	sort += time_span.count();
#endif

#ifdef CPU_BVH_MEASURE
	t1 = std::chrono::high_resolution_clock::now();
#endif
	
	// Divide node
	node.left = nullptr, node.right = nullptr;
	ge::sg::IndexedTriangleIterator splitPosition;

	splitPosition = divideBySAH(node, start, axis);

#ifdef CPU_BVH_MEASURE
	t2 = std::chrono::high_resolution_clock::now();
	time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	divide += time_span.count();
#endif

	// Left child
	if ((splitPosition - node.first) > 0) {
		ge::sg::AABB bvol;
		auto leftChild = std::make_shared<BVHNode>(bvol, node.first, splitPosition);
		node.left = leftChild;
		recursiveBuild(*leftChild, start, currentDepth - 1, axis == DivideAxis::X_AXIS ? DivideAxis::Y_AXIS :
			axis == DivideAxis::Y_AXIS ? DivideAxis::Z_AXIS :
			DivideAxis::X_AXIS);
	}

	// Right child
	if ((node.last - splitPosition) > 0) {
		ge::sg::AABB bvol;
		auto rightChild = std::make_shared<BVHNode>(bvol, splitPosition, node.last);
		node.right = rightChild;
		recursiveBuild(*rightChild, start, currentDepth - 1, axis == DivideAxis::X_AXIS ? DivideAxis::Y_AXIS :
			axis == DivideAxis::Y_AXIS ? DivideAxis::Z_AXIS :
			DivideAxis::X_AXIS);
	}

}

ge::sg::IndexedTriangleIterator ge::sg::AABB_SAH_BVH::divideBySAH(BVHNode & node, ge::sg::IndexedTriangleIterator & start, DivideAxis axis) {

	float currentSAH = std::numeric_limits<float>::max(), bestSAH = std::numeric_limits<float>::max();
	ge::sg::IndexedTriangleIterator result, tmp;

	float _min = axis == DivideAxis::X_AXIS ? node.volume.min.x :
		axis == DivideAxis::Y_AXIS ? node.volume.min.y : node.volume.min.z;
	float _max = axis == DivideAxis::X_AXIS ? node.volume.max.x :
		axis == DivideAxis::Y_AXIS ? node.volume.max.y : node.volume.max.z;
	float stepSize = (_max - _min) * (1.0f / nrOfPartitions);

	// Evaluating SAH in given number of steps
	for (int i = 1; i < nrOfPartitions; i++) {

		tmp = evaluateSAH(node, start, currentSAH, _min + (i * stepSize), i * stepSize, nrOfPartitions * stepSize, axis);
		
		assert(currentSAH > 0.0f);
		assert((tmp - node.first) >= 0);

		if (currentSAH < bestSAH) {
			result = tmp;
			
			bestSAH = currentSAH;
		}

	}

	if (((result - node.first) * 4) < (node.last - result) || ((node.last - result) * 4) < (result - node.first))
		result = node.first + ((node.last - node.first) / 2);


	return result;
}

ge::sg::IndexedTriangleIterator ge::sg::AABB_SAH_BVH::evaluateSAH(BVHNode & node, ge::sg::IndexedTriangleIterator & start, float & result, float criteria, float divSize, float boxSize, DivideAxis axis) {

	unsigned first = (node.first - start);
	unsigned last = (node.last - start);
	unsigned total = (node.last - node.first);
	unsigned cnt = 0;

	// Evaluating SAH for concrete situation
	for (auto it = first; it < last; it++, cnt++) {
	//for(auto it = node.first; it < node.last; ++it, cnt++, ij++){
		if (axis == DivideAxis::X_AXIS) {
			if (associatedCenters[it].center.x > criteria) {
				result = ((divSize / boxSize) * cnt) + (((boxSize - divSize) / boxSize) * (total - cnt));
				return (start + it);
			}
		}

		else if (axis == DivideAxis::Y_AXIS) {
			if (associatedCenters[it].center.y > criteria) {
				result = ((divSize / boxSize) * cnt) + (((boxSize - divSize) / boxSize) * (total - cnt));
				return (start + it);
				//return it;//(start + it);
			}
		}

		else if (axis == DivideAxis::Z_AXIS) {
			if (associatedCenters[it].center.z > criteria) {
				result = ((divSize / boxSize) * cnt) + (((boxSize - divSize) / boxSize) * (total - cnt));
				return (start + it);
				//return it;//(start + it);
			}
		}

	}
	
	result = ((divSize / boxSize) * cnt) + (((boxSize - divSize) / boxSize) * (total - cnt));
	return start + (last);

}
