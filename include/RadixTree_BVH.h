/*
* RayTracing pro GPUEngine
* Diplomova prace - master's thesis
* Bc. David Novák
* FIT VUT Brno
* 2018/2019
*
* RadixTree_BVH.h
*/

#pragma once

#include <GeneralGPUBVH.h>

namespace ge{
	namespace sg {

		/*
		* @brief Implementation of BVH builded on GPU 
		* @note Geometry in triangles form is expected
		*/
		class RadixTree_BVH : public GeneralGPUBVH {

		public:

			/*
			* @brief structure of BVH node on GPU
			*/
			typedef struct {
				glm::vec4 _min;
				glm::vec4 _max;
				int left;
				int right;
				int triangleA;
				int triangleB;
				glm::ivec4 ad;
			} bvh_node;

			/*
			* @brief Calls actual build of an BVH structure
			*/
			void build() override;

			/*
			* @brief Getter for BVH structure nodes
			* @return GL Buffer, which contains BVH nodes
			*/
			std::shared_ptr<ge::gl::Buffer> getNodes();

		//private:

			/*
			* @brief Initialization of build objects
			*/
			void init();

			/*
			* @brief Implementation of radix tree building process, in first phase it builds radix tree structure on sorted morton codes
			*		 in second phase it computes AABB bounding volume for every node
			*/
			void buildRadixTree();

			std::shared_ptr<ge::gl::Buffer> bvhNodes;	// Buffer with BVH nodes
			std::shared_ptr<ge::gl::Program> bvhKernel;	// Shader for BVH build

		};

	}
}
