/*
* RayTracing pro GPUEngine
* Diplomova prace - master's thesis
* Bc. David Novák
* FIT VUT Brno
* 2018/2019
*
* BVH.h
*/

#pragma once

#include <geSG/MeshTriangleIterators.h>
#include <geSG/MeshPrimitiveIterator.h>
#include <geSG/Mesh.h>
#include <geSG/Model.h>
#include <geSG/Scene.h>

#include <GeneralCPUBVH.h>
#include <GeneralGPUBVH.h>

#include <memory>


namespace ge {
	namespace sg {

		/*
		* @brief General class for work with BVH. It provides interface with common functions.
		* @note  It expects geometry data as a set of triangles in 3D space.
		*
		* Template type BuildPolicy - It specifies actual build policy. It's class inherited from GeneralCPUBVH class or GeneralGPUBVH class.
		*/
		template <typename BuildPolicy> class BVH : public BuildPolicy {

			using BuildPolicy::build;
			using BuildPolicy::setGeometry;
			using BuildPolicy::setMaxDepth;
			using BuildPolicy::setMinNodePrimitives;
			

		public:

			/**
			 * @brief Enumeration of BVH types, neccessary information for BVH traversal
			 *
			 * STANDARD_BVH - standard structure of BVH, primitives leaves in leaf nodes only
			 * RADIX_TREE_BVH - BVH based on radix tree structure, primitives can leaves in inner nodes too
			 */
			//idlist(StructType, STANDARD_BVH, RADIX_TREE_BVH);

			/**
			 * @brief Build BVH structure on given geometry data
			 */
			void buildBVH(){
				build();
			}

			/**
			 * @brief Setting geometry data for BVH
			 * @param data pointer to geometry data (coordinates)
			 * @param size number of coordiantes (number of dimensions * number of triangles)
			 */
			void setGeometryData(std::shared_ptr<float> data, size_t size) {
				BuildPolicy::setGeometry(data, size);
			}

			/**
			 * @brief Setting geometry data for BVH
			 * @param _start iterator pointing to first triangle
			 * @param _end iterator pointing to last triangle
			 */
			void setGeometryData(ge::sg::IndexedTriangleIterator& _start,
							     ge::sg::IndexedTriangleIterator& _end) {
				BuildPolicy::setGeometry(_start, _end);
			}

			/**
			 * @brief Setting geometry data for BVH
			 * @param _mesh mesh data for BVH
			 */
			void setGeometryData(ge::sg::Mesh& _mesh) {
				BuildPolicy::setGeometry(_mesh);
			}

			/**
			 * @brief Setting geometry data for BVH
			 * @param _model model for BVH
			 */
			void setGeometryData(ge::sg::Model& _model) {
				BuildPolicy::setGeometry(_model);
			}

			/**
			 * @brief Setting geometry data for BVH
			 * @param _scene scene for BVH
			 */
			void setGeometryData(ge::sg::Scene& _scene) {
				BuildPolicy::setGeometry(_scene);
			}

			/**
			 * @brief Sets maximum detph of BVH
			 * @param depth maximal number of BVH levels
			 */
			void setDepth(unsigned depth) {
				BuildPolicy::setMaxDepth(depth);
			}

			/**
			 * @brief Sets maximum allowed number of primitives in one node
			 * @param primitivesNumber maximum number of triangles in node
			 */
			void setMinimumPrimitivesInNode(unsigned primitivesNumber) {
				BuildPolicy::setMinNodePrimitives(primitivesNumber);
			}

		};

	}
}
