#include <SAH_BVH.h>

void ge::sg::SAH_BVH::computeCenters(ge::sg::IndexedTriangleIterator & _start, ge::sg::IndexedTriangleIterator & _end) {

	size_t i = 0;
	for (auto it = _start; it < _end; it += 1, i++) {

		primitiveCenter c;
		c.trIndex = i;
		c.center = (glm::make_vec3(it->v0) +
			glm::make_vec3(it->v1) +
			glm::make_vec3(it->v2)) / 3.0f;

		associatedCenters.push_back(c);

	}
}

void ge::sg::SAH_BVH::sortCenters(ge::sg::IndexedTriangleIterator & _start, ge::sg::IndexedTriangleIterator & _end, ge::sg::IndexedTriangleIterator & first, DivideAxis axis) {

	std::vector<float> temp = std::vector<float>((_end - _start) * _start.getN() * 3);
	unsigned beginOffset = (_start - first);
	unsigned lastOffset = (_end - first);

	if (!firstPass) {
		for (int i = 0; i < (_end - _start); i++) {
			(associatedCenters.begin() + beginOffset + i)->trIndex = i;
		}
	}

	// Sort by x axis
	if (axis == DivideAxis::X_AXIS) {
		std::sort(associatedCenters.begin() + beginOffset, associatedCenters.begin() + lastOffset,
			[&](primitiveCenter a, primitiveCenter b) {
			return a.center.x < b.center.x;
		});
	}

	// Sort by y axis
	else if (axis == DivideAxis::Y_AXIS) {
		std::sort(associatedCenters.begin() + beginOffset, associatedCenters.begin() + lastOffset,
			[&](primitiveCenter a, primitiveCenter b) {
			return a.center.y < b.center.y;
		});
	}

	// Sort by z axis
	else if (axis == DivideAxis::Z_AXIS) {
		std::sort(associatedCenters.begin() + beginOffset, associatedCenters.begin() + lastOffset,
			[&](primitiveCenter a, primitiveCenter b) {
			return a.center.z < b.center.z;
		});
	}

	// Reorder elements
	unsigned offset = 0, i = 0;
	for (auto it = associatedCenters.begin() + beginOffset; it < (associatedCenters.begin() + lastOffset); it++, i++) {
		std::memcpy(temp.data() + offset, (_start + it->trIndex)->v0, 3 * first.getN() * sizeof(float));
		offset += 3 * first.getN();
	}

	if (firstPass) firstPass = false;

	std::memcpy(_start->v0, temp.data(), temp.size() * sizeof(float));

}

void ge::sg::SAH_BVH::sortCentersIndexed(ge::sg::IndexedTriangleIterator & _start, ge::sg::IndexedTriangleIterator & _end, ge::sg::IndexedTriangleIterator & first, DivideAxis axis){

	std::vector<unsigned> temp = std::vector<unsigned>((_end - _start) * 3);
	unsigned beginOffset = (_start - first);
	unsigned lastOffset = (_end - first);
	
	if (!firstPass) {
		for (int i = 0; i < (_end - _start); i++) {
			(associatedCenters.begin() + beginOffset + i)->trIndex = i;
		}
	}

	// Sort by x axis
	if (axis == DivideAxis::X_AXIS) {
		std::sort(associatedCenters.begin() + beginOffset, associatedCenters.begin() + lastOffset,
			[&](primitiveCenter a, primitiveCenter b) {
			return a.center.x < b.center.x;
		});
	}

	// Sort by y axis
	else if (axis == DivideAxis::Y_AXIS) {
		std::sort(associatedCenters.begin() + beginOffset, associatedCenters.begin() + lastOffset,
			[&](primitiveCenter a, primitiveCenter b) {
			return a.center.y < b.center.y;
		});
	}

	// Sort by z axis
	else if (axis == DivideAxis::Z_AXIS) {
		std::sort(associatedCenters.begin() + beginOffset, associatedCenters.begin() + lastOffset,
			[&](primitiveCenter a, primitiveCenter b) {
			return a.center.z < b.center.z;
		});
	}

	// Reorder elements
	unsigned offset = 0;
	for (auto it = associatedCenters.begin() + beginOffset; it < (associatedCenters.begin() + lastOffset); it++) {
		std::memcpy(temp.data() + offset, (_start.getIndices() + (3 * it->trIndex)), 3 * sizeof(unsigned));
		offset += 3;
	}

	if (firstPass) firstPass = false;

	std::memcpy(first.getIndices() + (3 * beginOffset), temp.data(), temp.size() * sizeof(unsigned));

}
