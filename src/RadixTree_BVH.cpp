/*
* RayTracing pro GPUEngine
* Diplomova prace - master's thesis
* Bc. David Novák
* FIT VUT Brno
* 2018/2019
*
* RadixTree_BVH.cpp
*/

#include <RadixTree_BVH.h>
//#define GPU_BVH_MEASURE
//#define TREE_KERNEL "src/kernels/src/kernels/radixTreeBuilder.cs"

void ge::sg::RadixTree_BVH::build(){

	// Initialization
	initGPUObjects();
	init();

	// An building of BVH
	computeMortonCodes();	// compute morton codes
	sortMortonCodes();		// sort morton codes
	buildRadixTree();		// build radix tree based on sorted morton codes

}

std::shared_ptr<ge::gl::Buffer> ge::sg::RadixTree_BVH::getNodes(){
	return bvhNodes;
}

void ge::sg::RadixTree_BVH::init(){

	// Buffer containing BVH nodes
	bvhNodes = std::make_shared<ge::gl::Buffer>(sizeof(bvh_node) * ((inputData.size() / 9) - 1));
	bvhNodes->setData(nullptr);

	// Shader for BVH build
	auto treeShader = std::make_shared<ge::gl::Shader>(GL_COMPUTE_SHADER, ge::core::loadTextFile(TREE_KERNEL));
	bvhKernel = std::make_shared<ge::gl::Program>(treeShader);

}

void ge::sg::RadixTree_BVH::buildRadixTree(){

	unsigned count = inputData.size() / 9;

#ifdef GPU_BVH_MEASURE
	GLuint query;
	int done = 0;
	GLuint64 elapsed_time;
	
	ge::gl::glGenQueries(1, &query);
	ge::gl::glBeginQuery(GL_TIME_ELAPSED, query);
#endif // GPU_BVH_MEASURE

	// --- Phase 3 - Structure build ---

	bvhKernel->use();

	mortonCodes->bindBase(GL_SHADER_STORAGE_BUFFER, 11);
	bvhNodes->bindBase(GL_SHADER_STORAGE_BUFFER, 14);
	indicesBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 12);
	verticesBuffer->bindBase(GL_SHADER_STORAGE_BUFFER, 10);

	GLint wgs[3];
	bvhKernel->getComputeWorkGroupSize(wgs);

	bvhKernel->set1i("size", count - 1);
	bvhKernel->set1i("phase", 0);

	ge::gl::glDispatchCompute((int)ceil((count) / static_cast<float>(wgs[0])), 1, 1);
	ge::gl::glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	bvhKernel->set1i("phase", 1);
	
	ge::gl::glDispatchCompute((int)ceil((count) / static_cast<float>(wgs[0])), 1, 1);
	ge::gl::glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// --- Phase 3 - Structure build ---

	mortonCodes->unbindBase(GL_SHADER_STORAGE_BUFFER, 11);
	bvhNodes->unbindBase(GL_SHADER_STORAGE_BUFFER, 14);
	indicesBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 12);
	verticesBuffer->unbindBase(GL_SHADER_STORAGE_BUFFER, 10);

#ifdef GPU_BVH_MEASURE
	ge::gl::glEndQuery(GL_TIME_ELAPSED);

	while (!done)
		ge::gl::glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &done);

	done = 0;
	ge::gl::glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsed_time);

	std::cout << "BVH build " << (elapsed_time / 1000000) << "ms" << std::endl;
#endif // GPU_BVH_MEASURE

}
