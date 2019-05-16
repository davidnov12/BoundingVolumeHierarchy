/*
* RayTracing pro GPUEngine
* Diplomova prace - master's thesis
* Bc. David Novák
* FIT VUT Brno
* 2018/2019
*
* GeneralGPUBVH.h
*/

#pragma once

#include <geSG/MeshTriangleIterators.h>
#include <geSG/MeshPrimitiveIterator.h>
#include <geSG/Mesh.h>
#include <geSG/Model.h>
#include <geSG/Scene.h>

#include <geGL/geGL.h>
#include <geGL/StaticCalls.h>

#include <geCore/Text.h>

#include <memory>
#include <vector>
#include <string>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GPU_BVH_MEASEURE

namespace ge {
	namespace sg {

		/**
		* @brief Implementation of common operation for BVH build on GPU
		* @note 
		*/
		class GeneralGPUBVH {

		public:

			/**
			* @brief Build function
			*/
			virtual void build() {}

			/**
			* @brief
			* @param data first primitive
			* @param size last primitive
			*/
			void setGeometry(std::shared_ptr<float> data, size_t size);

			/**
			* @brief 
			* @param _start first primitive
			* @param _end last primitive
			*/
			void setGeometry(ge::sg::IndexedTriangleIterator& _start,
				ge::sg::IndexedTriangleIterator& _end);


			/**
			* @brief
			* @param _geometry geometry for BVH
			*/
			void setGeometry(ge::sg::Mesh& _geometry);

			/**
			* @brief
			* @param _geometry geometry for BVH
			*/
			void setGeometry(ge::sg::Model& _geometry);

			/**
			* @brief 
			* @param _geometry geometry for BVH
			*/
			void setGeometry(ge::sg::Scene& _geometry);

			/**
			* @brief 
			* @param _depth maximum depth of BVH
			* @note empty implementation, depth of BVH is derived by input data size
			*/
			void setMaxDepth(unsigned _depth){}


			/**
			* @brief 
			* @param _minNodePrimitives minimum number of primitives in node
			* @note empty implementation, minimum number of primitives in node is 2 (property of radix tree)
			*/
			void setMinNodePrimitives(unsigned _minNodePrimitives){}


			/**
			* @brief Getter for trinagles indices
			* @return Buffer with triangles indices
			*/
			std::shared_ptr<ge::gl::Buffer> getIndices();

		//protected:

			/**
			* @brief Initialization of GPU objects
			*/
			void initGPUObjects();

			/**
			* @brief Generates vector for given amount of triangles
			* @param numberOfTriangles Amount of triangles
			* @return Initial vector of triangles
			*/
			std::vector<unsigned> generateIndices(size_t numberOfTriangles);

			/**
			* @brief Computes minimum and maximum coordinates of given geometry
			* @return pair of minimum + maximum coordinate
			*/
			std::pair<glm::vec3, glm::vec3> findMinMaxCoords();

			/**
			* @brief Computes morton codes from given geometry on GPU
			* @note
			*/
			void computeMortonCodes();


			/**
			* @brief Sort morton codes on GPU using parallel radix sort
			* @note 
			*/
			void sortMortonCodes();

			// Common attributes (index & vertex buffers, helper buffers for BVH build)
			std::shared_ptr<ge::gl::Buffer> verticesBuffer, indicesBuffer, mortonCodes, radixBucket;	// Buffers
			std::shared_ptr<ge::gl::Program> mortonKernel, sortKernel;									// Kernels (compute shaders respectivelly)
			std::vector<float> inputData = std::vector<float>();										// Vector of input geometry data 

		};

	}
}