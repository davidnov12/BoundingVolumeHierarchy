#include <SAH_BVH.h>

void ge::sg::SAH_BVH::computeCenters(ge::sg::IndexedTriangleIterator & _start, ge::sg::IndexedTriangleIterator & _end) {

	size_t i = 0;
	for (auto it = _start; it <= _end; it += 1, i++) {

		primitiveCenter c;
		c.trIndex = i;
		c.center = (glm::make_vec3(it->v0) +
			glm::make_vec3(it->v1) +
			glm::make_vec3(it->v2)) / 3.0f;

		associatedCenters.push_back(c);

		//printf("%f %f %f\n", c.center.x, c.center.y, c.center.z);

	}

}

void ge::sg::SAH_BVH::sortCenters(ge::sg::IndexedTriangleIterator & _start, ge::sg::IndexedTriangleIterator & _end, ge::sg::IndexedTriangleIterator & first, DivideAxis axis) {

	std::vector<float> temp = std::vector<float>((_end - _start + 1) * _start.getN() * 3);
	unsigned beginOffset = (_start - first);
	unsigned lastOffset = (_end - first) + 1;

	if (!firstPass) {
		for (int i = 0; i < (_end - _start + 1); i++) {
			(associatedCenters.begin() + beginOffset + i)->trIndex = i;
		}
	}

	/*printf("\n-----------------------------------\n");
	//printf("%d %d\n", beginOffset, lastOffset);
	//printf("unsorted\n");
	for (int i = 0; i < temp.size(); i++) {
		if (i % 9 == 0) printf("\n");
		printf("%.2f ", _start->v0[i]);
	}
	printf("\n");*/
	//for (int i = 0; i < associatedCenters.size(); i++) {
		
	//	printf("%d\n ", associatedCenters[i].trIndex);
	//}
	
	//printf("\n");
	//printf("ts %d\n", temp.size());
	//printf("sort\n");
	/*for (int i = beginOffset; i < lastOffset; i++) {
		if (axis == DivideAxis::X_AXIS) printf("%f\n", associatedCenters[i].center.x);
		if (axis == DivideAxis::Y_AXIS) printf("%f\n", associatedCenters[i].center.y);
		if (axis == DivideAxis::Z_AXIS) printf("%f\n", associatedCenters[i].center.z);
	}*/
	//printf("------------------\n");
	// Sort by x axis
	if (axis == DivideAxis::X_AXIS) {
		std::sort(associatedCenters.begin() + beginOffset, associatedCenters.begin() + lastOffset,
			[&](primitiveCenter a, primitiveCenter b) {
			//printf("sorting %f %f\n", a.center.x, b.center.x);
			return a.center.x < b.center.x;
		});
	}

	// Sort by y axis
	else if (axis == DivideAxis::Y_AXIS) {
		std::sort(associatedCenters.begin() + beginOffset, associatedCenters.begin() + lastOffset,
			[&](primitiveCenter a, primitiveCenter b) {
			//printf("sorting %f %f\n", a.center.y, b.center.y);
			return a.center.y < b.center.y;
		});
	}

	// Sort by z axis
	else if (axis == DivideAxis::Z_AXIS) {
		std::sort(associatedCenters.begin() + beginOffset, associatedCenters.begin() + lastOffset,
			[&](primitiveCenter a, primitiveCenter b) {
			//printf("sorting %f %f\n", a.center.z, b.center.z);
			return a.center.z < b.center.z;
		});
	}

	// Reorder elements
	unsigned offset = 0, i = 0;
	for (auto it = associatedCenters.begin() + beginOffset; it < associatedCenters.begin() + lastOffset; it++, i++) {
		
		//if (firstPass) std::memcpy(temp.data() + offset, (first + it->trIndex)->v0, 3 * first.getN() * sizeof(float));
		std::memcpy(temp.data() + offset, (_start + it->trIndex)->v0, 3 * first.getN() * sizeof(float));
		offset += 3 * first.getN();
	}

	if (firstPass) firstPass = false;

	//printf("%d %d\n", temp.size(), (lastOffset - beginOffset - 1));
	//assert(temp.size() == (lastOffset - beginOffset) * 3);
	//printf("BYTES %d\n", temp.size() * sizeof(float));
	//printf("%d %f\n", _start->v0, _start->v0[0]);
	std::memcpy(_start->v0, temp.data(), temp.size() * sizeof(float));
	//printf("%d %f\n", _start->v0, _start->v0[0]);
	/*for (int i = 0; i < temp.size(); i++){
		if (i % 9 == 0) printf("\n");
		printf("%.2f ", _start->v0[i]);
	}
	printf("\n-----------------------------------\n");*/
	//printf("------------------\n");
	/*for (int i = beginOffset; i < lastOffset; i++) {
		if (axis == DivideAxis::X_AXIS) printf("%f\n", associatedCenters[i].center.x);
		if (axis == DivideAxis::Y_AXIS) printf("%f\n", associatedCenters[i].center.y);
		if (axis == DivideAxis::Z_AXIS) printf("%f\n", associatedCenters[i].center.z);
	}*/

	//temp.shrink_to_fit();

}

void ge::sg::SAH_BVH::sortCentersIndexed(ge::sg::IndexedTriangleIterator & _start, ge::sg::IndexedTriangleIterator & _end, ge::sg::IndexedTriangleIterator & first, DivideAxis axis){

	std::vector<unsigned> temp = std::vector<unsigned>((_end - _start + 1) * 3);
	unsigned beginOffset = (_start - first);
	unsigned lastOffset = (_end - first) + 1;
	
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
	for (auto it = associatedCenters.begin() + beginOffset; it < associatedCenters.begin() + lastOffset; it++) {
		std::memcpy(temp.data() + offset, (first.getIndices() + it->trIndex), 3 * sizeof(unsigned));
		offset += 3;
	}

	std::memcpy(first.getIndices() + (sizeof(unsigned) * beginOffset), temp.data(), temp.size() * sizeof(unsigned));

}
