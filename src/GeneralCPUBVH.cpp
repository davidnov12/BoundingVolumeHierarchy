/*
* RayTracing pro GPUEngine
* Diplomova prace - master's thesis
* Bc. David Novák
* FIT VUT Brno
* 2018/2019
*
* GeneralCPUBVH.cpp
*/

#include <GeneralCPUBVH.h>

void ge::sg::GeneralCPUBVH::setGeometry(std::shared_ptr<float> data, size_t size){

	assert((size % 9) == 0);

	_firstPrimitive = ge::sg::IndexedTriangleIterator(data.get(), nullptr, 3);
	_lastPrimitive = ge::sg::IndexedTriangleIterator(&(data.get())[size], nullptr, 3);

}

void ge::sg::GeneralCPUBVH::setGeometry(ge::sg::IndexedTriangleIterator & _start, ge::sg::IndexedTriangleIterator & _end){

	_firstPrimitive = _start;
	_lastPrimitive = _end;

}

void ge::sg::GeneralCPUBVH::setGeometry(ge::sg::Mesh & _geometry){

	auto _start = ge::sg::MeshPositionIteratorBegin(&_geometry);
	auto _end = ge::sg::MeshPositionIteratorEnd(&_geometry);

	_firstPrimitive = _start;
	_lastPrimitive = _end;

}

void ge::sg::GeneralCPUBVH::setGeometry(ge::sg::Model & _geometry){

	if(_geometry.meshes.size() > 0)
		_firstPrimitive = ge::sg::MeshPositionIteratorBegin(_geometry.meshes[0].get());
		
	for (auto mesh : _geometry.meshes) {
		_lastPrimitive = ge::sg::MeshPositionIteratorEnd(mesh.get());
	}

}

void ge::sg::GeneralCPUBVH::setGeometry(ge::sg::Scene & _geometry){

	if (_geometry.models.size() > 0)
		if(_geometry.models[0]->meshes.size() > 0)
			_firstPrimitive = ge::sg::MeshPositionIteratorBegin(_geometry.models[0]->meshes[0].get());
	
	for (auto model : _geometry.models) {
		for (auto mesh : model->meshes) {
			_lastPrimitive = ge::sg::MeshPositionIteratorEnd(mesh.get());
		}
	}

}

void ge::sg::GeneralCPUBVH::setMaxDepth(unsigned _depth){

	maxDepth = _depth;

}

void ge::sg::GeneralCPUBVH::setMinNodePrimitives(unsigned _minNodePrimitives){

	minVolumePrimitives = _minNodePrimitives;

}

void ge::sg::GeneralCPUBVH::computeCenters(ge::sg::IndexedTriangleIterator & _start, ge::sg::IndexedTriangleIterator & _end){

	for (auto it = _start; it < _end; it += 1) {

		primitiveCenter c;
		c.center = (glm::make_vec3(it->v0) +
			glm::make_vec3(it->v1) +
			glm::make_vec3(it->v2)) / 3.0f;

		associatedCenters.push_back(c);

	}

}


void ge::sg::GeneralCPUBVH::sortCenters(ge::sg::IndexedTriangleIterator & _start, ge::sg::IndexedTriangleIterator & _end, ge::sg::IndexedTriangleIterator & first, DivideAxis axis){

	std::vector<float> temp = std::vector<float>((_end - _start) * _start.getN() * 3);
	unsigned beginOffset = (_start - first);
	unsigned lastOffset = (_end - first);

	// Prepare triangles indices
	for (int i = 0; i < (_end - _start); i++) {
		(associatedCenters.begin() + beginOffset + i)->trIndex = i;
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
	for (auto it = associatedCenters.begin() + beginOffset; it <= (associatedCenters.begin() + lastOffset); it++, i++) {
		std::memcpy(temp.data() + offset, (_start + it->trIndex)->v0, 3 * first.getN() * sizeof(float));
		offset += 3 * first.getN();
	}

	// Copy coordinates back
	std::memcpy(_start->v0, temp.data(), temp.size() * sizeof(float));

}

void ge::sg::GeneralCPUBVH::sortCentersIndexed(ge::sg::IndexedTriangleIterator & _start, ge::sg::IndexedTriangleIterator & _end, ge::sg::IndexedTriangleIterator & first, DivideAxis axis){

	std::vector<unsigned> temp = std::vector<unsigned>((_end - _start) * 3);
	unsigned beginOffset = (_start - first);
	unsigned lastOffset = (_end - first);

	// Prepare triangles indices
	for (int i = 0; i <= (_end - _start); i++) {
		(associatedCenters.begin() + beginOffset + i)->trIndex = i;
		
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

	// Copy indices back
	std::memcpy(first.getIndices() + (3 * beginOffset), temp.data(), temp.size() * sizeof(unsigned));

}
