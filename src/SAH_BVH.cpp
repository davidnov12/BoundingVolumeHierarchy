#include <SAH_BVH.h>

void ge::sg::SAH_BVH::computeCenters(ge::sg::TriangleIterator & _start, ge::sg::TriangleIterator & _end) {

	size_t i = 0;
	for (auto it = _start; it != _end; it += 1, i++) {

		primitiveCenter c;
		c.trIndex = i;
		c.center = (glm::make_vec3(it->v0) +
			glm::make_vec3(it->v1) +
			glm::make_vec3(it->v2)) / 3.0f;

		associatedCenters.push_back(c);

	}

}

void ge::sg::SAH_BVH::sortCenters(ge::sg::TriangleIterator & _start, ge::sg::TriangleIterator & _end, ge::sg::TriangleIterator & first, DivideAxis axis) {

	std::vector<float> temp = std::vector<float>((_end - _start) * _start.getN());
	unsigned beginOffset = ((_start - first) / _start.getN());
	unsigned lastOffset = ((_end - first) / _end.getN());

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
	for (auto it = associatedCenters.begin() + beginOffset; it != associatedCenters.begin() + lastOffset; it++) {
		std::memcpy(temp.data() + offset, (first + it->trIndex)->v0, 3 * first.getN() * sizeof(float));
		offset += 3 * first.getN();
	}

	std::memcpy(_start->v0, temp.data(), temp.size() * sizeof(float));

	temp.shrink_to_fit();

}
