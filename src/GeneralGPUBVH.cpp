/*
* RayTracing pro GPUEngine
* Diplomova prace - master's thesis
* Bc. David Novák
* FIT VUT Brno
* 2018/2019
*
* GeneralGPUBVH.cpp
*/

#include "GeneralGPUBVH.h"
//#define GPU_BVH_MEASURE

void ge::sg::GeneralGPUBVH::setGeometry(std::shared_ptr<float> data, size_t size){

	inputData.shrink_to_fit();
	inputData.resize(size);
	std::memcpy(inputData.data(), data.get(), size * sizeof(float));

}

void ge::sg::GeneralGPUBVH::setGeometry(ge::sg::IndexedTriangleIterator & _start, ge::sg::IndexedTriangleIterator & _end){

	unsigned count = _end - _start;
	inputData.shrink_to_fit();
	inputData.resize(count * 9);
	std::memcpy(inputData.data(), _start->v0, 9 * count * sizeof(float));

}

void ge::sg::GeneralGPUBVH::setGeometry(ge::sg::Mesh & _geometry){

	auto _start = ge::sg::MeshPositionIteratorBegin(&_geometry);
	auto _end = ge::sg::MeshPositionIteratorEnd(&_geometry);

	unsigned count = _end - _start;
	inputData.shrink_to_fit();
	inputData.resize(count * 9);
	std::memcpy(inputData.data(), _start->v0, 9 * count * sizeof(float));

}

void ge::sg::GeneralGPUBVH::setGeometry(ge::sg::Model & _geometry){

	unsigned offset = 0;
	inputData.shrink_to_fit();

	for (auto mesh : _geometry.meshes) {

		auto _start = ge::sg::MeshPositionIteratorBegin(mesh.get());
		auto _end = ge::sg::MeshPositionIteratorEnd(mesh.get());

		unsigned count = _end - _start;
		
		inputData.resize(inputData.size() + (count * 9));
		std::memcpy(inputData.data() + offset, _start->v0, 9 * count * sizeof(float));

		offset += 9 * count;
	}

}

void ge::sg::GeneralGPUBVH::setGeometry(ge::sg::Scene & _geometry){

	unsigned offset = 0;
	inputData.shrink_to_fit();

	for(auto model : _geometry.models){
		for (auto mesh : model->meshes) {

			auto _start = ge::sg::MeshPositionIteratorBegin(mesh.get());
			auto _end = ge::sg::MeshPositionIteratorEnd(mesh.get());

			unsigned count = _end - _start;
			
			inputData.resize(inputData.size() + (count * 9));
			std::memcpy(inputData.data() + offset, _start->v0, 9 * count * sizeof(float));

			offset += 9 * count;
		}
	}

}

std::shared_ptr<ge::gl::Buffer> ge::sg::GeneralGPUBVH::getIndices(){
	return indicesBuffer;
}

void ge::sg::GeneralGPUBVH::initGPUObjects(){

	verticesBuffer = std::make_shared<ge::gl::Buffer>(sizeof(float) * inputData.size());
	verticesBuffer->setData(inputData.data(), inputData.size() * sizeof(float), 0);

	verticesBuffer->setData(inputData.data(), inputData.size() * sizeof(float), 0);

	// Buffer with indices of triangles
	std::vector<unsigned> indices(inputData.size() / 9);

	for (unsigned i = 0; i < inputData.size() / 9; i++)
		indices[i] = i;

	//auto indices = generateIndices(inputData.size() / 9);
	indicesBuffer = std::make_shared<ge::gl::Buffer>(2 * sizeof(unsigned) * indices.size());
	indicesBuffer->setData(indices.data(), indices.size() * sizeof(unsigned), 0);
	indicesBuffer->setData(indices.data(), indices.size() * sizeof(unsigned), indices.size() * sizeof(unsigned));

	// Buffer for morton codes of triangles centroids
	mortonCodes = std::make_shared<ge::gl::Buffer>(2 * sizeof(unsigned) * (inputData.size() / 9));
	mortonCodes->setData(nullptr);

	// Buffer for parallel radix sort (histogram computing, reordering elements)
	radixBucket = std::make_shared<ge::gl::Buffer>(4 * sizeof(unsigned) * (inputData.size() / 9));
	radixBucket->setData(nullptr);


	auto mortonShader = std::make_shared<ge::gl::Shader>(GL_COMPUTE_SHADER, ge::core::loadTextFile("../src/BVH/kernels/mortonCodeComputation.cs"));
	mortonKernel = std::make_shared<ge::gl::Program>(mortonShader);

	auto sortShader = std::make_shared<ge::gl::Shader>(GL_COMPUTE_SHADER, ge::core::loadTextFile("../src/BVH/kernels/parallelRadixSort.cs"));
	sortKernel = std::make_shared<ge::gl::Program>(sortShader);

}

std::vector<unsigned> ge::sg::GeneralGPUBVH::generateIndices(size_t numberOfTriangles){

	std::vector<unsigned> indices(numberOfTriangles);

	for (unsigned i = 0; i < numberOfTriangles; i++)
		indices[i] = i;

	return indices;
}

std::pair<glm::vec3, glm::vec3> ge::sg::GeneralGPUBVH::findMinMaxCoords(){

	glm::vec3 minCoord = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxCoord = glm::vec3(-std::numeric_limits<float>::max());

	for (int i = 0; i < inputData.size(); i += 3) {
		minCoord.x = std::min(minCoord.x, inputData.at(i));
		minCoord.y = std::min(minCoord.y, inputData.at(i+1));
		minCoord.z = std::min(minCoord.z, inputData.at(i+2));

		maxCoord.x = std::max(maxCoord.x, inputData.at(i));
		maxCoord.y = std::max(maxCoord.y, inputData.at(i+1));
		maxCoord.z = std::max(maxCoord.z, inputData.at(i+2));
	}

	return std::pair<glm::vec3, glm::vec3>(minCoord, maxCoord);
}

void ge::sg::GeneralGPUBVH::computeMortonCodes(){

	unsigned count = inputData.size() / 9;
	auto minMax = findMinMaxCoords();

#ifdef GPU_BVH_MEASURE
	GLuint query;
	int done = 0;
	GLuint64 elapsed_time;
	ge::gl::glGenQueries(1, &query);
	ge::gl::glBeginQuery(GL_TIME_ELAPSED, query);
#endif // GPU_BVH_MEASURE

	// --- Phase 1 - Morton code calculation ---
	mortonKernel->use();
	verticesBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 10);
	mortonCodes->bindBase(GL_SHADER_STORAGE_BUFFER, 11);

	mortonKernel->set3f("minCoord", minMax.first.x, minMax.first.y, minMax.first.z);
	mortonKernel->set3f("maxCoord", minMax.second.x, minMax.second.y, minMax.second.z);
	mortonKernel->set1i("numberOfTriangles", count);

	GLint wgs[3];
	mortonKernel->getComputeWorkGroupSize(wgs);
	ge::gl::glDispatchCompute(ceil(count / static_cast<float>(wgs[0])), 1, 1);
	ge::gl::glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	verticesBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 10);
	mortonCodes->unbindBase(GL_SHADER_STORAGE_BUFFER, 11);
	// --- Phase 1 - Morton code calculation ---

#ifdef GPU_BVH_MEASURE
	ge::gl::glEndQuery(GL_TIME_ELAPSED);

	while (!done)
		ge::gl::glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &done);

	done = 0;
	ge::gl::glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsed_time);
	std::cout << "Morton codes computuation " << (elapsed_time / 1000000) << "ms" << std::endl;
#endif // GPU_BVH_MEASURE


}

void ge::sg::GeneralGPUBVH::sortMortonCodes(){

	unsigned count = inputData.size() / 9;

#ifdef GPU_BVH_MEASURE
	GLuint query;
	int done = 0;
	GLuint64 elapsed_time;
	ge::gl::glGenQueries(1, &query);
	ge::gl::glBeginQuery(GL_TIME_ELAPSED, query);
#endif // GPU_BVH_MEASURE

	// --- Phase 2 - Morton code sort ---

	int scan_steps = ceil(log2f(2 * count));
	int offsetTable[] = { 0, count };
	int activeIn = 0;

	sortKernel->use();
	sortKernel->set1i("size", count);

	radixBucket->bindBase(GL_SHADER_STORAGE_BUFFER, 13);
	mortonCodes->bindBase(GL_SHADER_STORAGE_BUFFER, 11);
	indicesBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 12);

	GLint wgs[3];
	sortKernel->getComputeWorkGroupSize(wgs);

	// Parallel Radix sort with 30 iterations (30 bit morton codes)
	for (int i = 0; i < 30; i++) {

		// --Histogram--
		sortKernel->set1i("phase", 0);
		sortKernel->set1i("base", 0x1 << i);
		sortKernel->set1i("inOffset", offsetTable[activeIn]);
		sortKernel->set1i("outOffset", offsetTable[1 - activeIn]);

		ge::gl::glDispatchCompute((int)ceil(count / static_cast<float>(wgs[0])), 1, 1);
		ge::gl::glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		// --Histogram--

		// --Prefix sum--
		sortKernel->set1i("size", 2 * count);
		sortKernel->set1i("phase", 1);

		for (int j = 0; j < scan_steps; j++) {
			sortKernel->set1i("delta", 0x1 << j);
			sortKernel->set1i("inBucket", (j % 2) * 2 * count);
			sortKernel->set1i("outBucket", ((j + 1) % 2) * 2 * count);

			ge::gl::glDispatchCompute((int)ceil((2 * count) / static_cast<float>(wgs[0])), 1, 1);
			ge::gl::glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		}
		// --Prefix sum--

		// --Reorder--
		sortKernel->set1i("size", count);
		sortKernel->set1i("phase", 2);

		ge::gl::glDispatchCompute(ceil(count / static_cast<float>(wgs[0])), 1, 1);
		ge::gl::glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		// --Reorder--

		activeIn = 1 - activeIn;
	}

	radixBucket->unbindBase(GL_SHADER_STORAGE_BUFFER, 13);
	mortonCodes->unbindBase(GL_SHADER_STORAGE_BUFFER, 11);
	indicesBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 12);
	// --- Phase 2 - Morton code sort ---

#ifdef GPU_BVH_MEASURE
	ge::gl::glEndQuery(GL_TIME_ELAPSED);

	while (!done)
		ge::gl::glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &done);

	done = 0;
	ge::gl::glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsed_time);
	std::cout << "Radix sort " << (elapsed_time / 1000000) << "ms" << std::endl;
#endif // GPU_BVH_MEASURE

}
