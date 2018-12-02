#include <SAH_AABB_BVH.h>

using BVHNode = ge::sg::BVH_Node<ge::sg::AABB>;

ge::sg::SAH_AABB_BVH::SAH_AABB_BVH(ge::sg::IndexedTriangleIterator & _start, ge::sg::IndexedTriangleIterator & _end, unsigned _depth, float _binLength) {

	binLength = std::max(0.00001f, std::min(1.0f, _binLength));
	maxDepth = _depth;
	computeCenters(_start, _end);
	build(_start, _end);

}

ge::sg::SAH_AABB_BVH::SAH_AABB_BVH(ge::sg::Mesh & _geometry, unsigned _depth, float _binLength){

	binLength = std::max(0.00001f, std::min(1.0f, _binLength));
	maxDepth = _depth;

	ge::sg::IndexedTriangleIterator _start = ge::sg::MeshPositionIteratorBegin(&_geometry);
	ge::sg::IndexedTriangleIterator _end = ge::sg::MeshPositionIteratorEnd(&_geometry);

	computeCenters(_start, _end);
	build(_start, _end);
	// ...

}

void ge::sg::SAH_AABB_BVH::build(ge::sg::IndexedTriangleIterator & _start, ge::sg::IndexedTriangleIterator & _end){

	ge::sg::AABB bvol;
	rootNode = std::make_shared<BVHNode>(bvol, _start, _end);
	
	recursiveBuild(*rootNode, _start, maxDepth - 1, DivideAxis::X_AXIS);

}

std::shared_ptr<BVHNode> ge::sg::SAH_AABB_BVH::getRoot(){
	return rootNode;
}

void ge::sg::SAH_AABB_BVH::recursiveBuild(BVHNode & node, ge::sg::IndexedTriangleIterator & start, unsigned currentDepth, DivideAxis axis){


	// Bounding Volume refit
	glm::vec3 _min(std::numeric_limits<float>::max()), _max(std::numeric_limits<float>::min());
	for (auto i = node.first; i < node.last; ++i) {
		_min.x = std::min(_min.x, std::min(i->v0[0], std::min(i->v1[0], i->v2[0])));
		_min.y = std::min(_min.y, std::min(i->v0[1], std::min(i->v1[1], i->v2[1])));
		_min.z = std::min(_min.z, std::min(i->v0[2], std::min(i->v1[2], i->v2[2])));

		_max.x = std::max(_max.x, std::max(i->v0[0], std::max(i->v1[0], i->v2[0])));
		_max.y = std::max(_max.y, std::max(i->v0[1], std::max(i->v1[1], i->v2[1])));
		_max.z = std::max(_max.z, std::max(i->v0[2], std::max(i->v1[2], i->v2[2])));
	}
	
	node.volume.min = _min;
	node.volume.max = _max;
	
	// Recursion end - max depth reached
	if (currentDepth == 0) {
		return;
	}

	// Sort centers by one axis
	if(node.first.getIndices() == nullptr)
		sortCenters(node.first, node.last, start, axis);
	else
		sortCentersIndexed(node.first, node.last, start, axis);
	
	// Divide node
	node.left = nullptr, node.right = nullptr;
	ge::sg::IndexedTriangleIterator splitPosition;
	splitPosition = divideBySAH(node, start, axis);
	
	// Left child
	if ((splitPosition - node.first) > 0) {
		ge::sg::AABB bvol;
		auto leftChild = std::make_shared<BVHNode>(bvol, node.first, splitPosition + (0));
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

ge::sg::IndexedTriangleIterator ge::sg::SAH_AABB_BVH::divideBySAH(BVHNode & node, ge::sg::IndexedTriangleIterator & start, DivideAxis axis) {
	
	float currentSAH = std::numeric_limits<float>::max(), bestSAH = std::numeric_limits<float>::max();
	ge::sg::IndexedTriangleIterator result, tmp;

	float _min = axis == DivideAxis::X_AXIS ? node.volume.min.x :
				 axis == DivideAxis::Y_AXIS ? node.volume.min.y : node.volume.min.z;
	float _max = axis == DivideAxis::X_AXIS ? node.volume.max.x :
				 axis == DivideAxis::Y_AXIS ? node.volume.max.y : node.volume.max.z;
	float stepSize = (_max - _min) * binLength;
	int steps = 1.0f / binLength;
	
	// Evaluating SAH in given number of steps
	for (int i = 1; i < steps; i++) {
		
		tmp = evaluateSAH(node, start, currentSAH, _min + (i * stepSize), i * stepSize, axis);

		if (currentSAH < bestSAH) {
			result = tmp;
			bestSAH = currentSAH;
		}

	}

	return result;

}

ge::sg::IndexedTriangleIterator ge::sg::SAH_AABB_BVH::evaluateSAH(BVHNode & node, ge::sg::IndexedTriangleIterator & start, float & result, float criteria, float divSize, DivideAxis axis) {
	
	unsigned first = (node.first - start);
	unsigned last = (node.last - start);
	unsigned total = (node.last - node.first) + 1;
	unsigned cnt = 0;

	// Evaluating SAH for concrete situation
	for (auto it = first; it <= last; it++, cnt++) {

		if (axis == DivideAxis::X_AXIS) {
			if (associatedCenters[it].center.x > criteria) {
				result = (divSize * cnt) + ((1.0f - divSize) * (total - cnt));
				return (start + it);
			}
		}

		else if (axis == DivideAxis::Y_AXIS) {
			if (associatedCenters[it].center.y > criteria) {
				result = (divSize * cnt) + ((1.0f - divSize) * (total - cnt));
				return (start + it);
			}
		}

		else if (axis == DivideAxis::Z_AXIS) {
			if (associatedCenters[it].center.z > criteria) {
				result = (divSize * cnt) + ((1.0f - divSize) * (total - cnt));
				return (start + it);
			}
		}

	}

	result = (divSize * cnt) + ((1.0f - divSize) * (total - cnt));
	return start + last;

}
