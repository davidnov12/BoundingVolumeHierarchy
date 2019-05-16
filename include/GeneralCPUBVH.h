/*
* RayTracing pro GPUEngine
* Diplomova prace - master's thesis
* Bc. David Novák
* FIT VUT Brno
* 2018/2019
*
* GeneralCPUBVH.h
*/

#pragma once

#include <geSG/MeshTriangleIterators.h>
#include <geSG/MeshPrimitiveIterator.h>
#include <geSG/Mesh.h>
#include <geSG/Model.h>
#include <geSG/Scene.h>

#include <geCore/idlist.h>

#include <algorithm>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace ge {
	namespace sg {

		/*
		 * @brief Class with common attributes and functions for BVH build on CPU
		 */
		class GeneralCPUBVH {

		public:

			// Enumeration of dividing axises
			idlist(DivideAxis, X_AXIS, Y_AXIS, Z_AXIS);

			// Build function
			virtual void build() {}

			// Centroids structure
			typedef struct {
				unsigned trIndex;
				glm::vec3 center;
				unsigned mortonCode;
			} primitiveCenter;

			// Common attributes
			unsigned maxDepth = 10;
			unsigned dividePartitions = 10;
			unsigned minVolumePrimitives = 10;
			
			std::vector<primitiveCenter> associatedCenters;
			ge::sg::IndexedTriangleIterator _firstPrimitive, _lastPrimitive;


			/*
			* @param _start - first primitive
			* @param _end - last primitive
			*/
			void setGeometry(std::shared_ptr<float> data, size_t size);

			/*
			* @param _start - first primitive
			* @param _end - last primitive
			*/
			void setGeometry(ge::sg::IndexedTriangleIterator& _start,
				ge::sg::IndexedTriangleIterator& _end);


			/*
			* @param _geometry - geometry for BVH
			*/
			void setGeometry(ge::sg::Mesh& _geometry);

			/*
			* @param _geometry - geometry for BVH
			*/
			void setGeometry(ge::sg::Model& _geometry);

			/*
			* @param _geometry - geometry for BVH
			*/
			void setGeometry(ge::sg::Scene& _geometry);

			/*
			* @param _depth - maximum depth of BVH
			*/
			void setMaxDepth(unsigned _depth);


			/*
			* @param _minNodePrimitives - minimum number of primitives in node
			*/
			void setMinNodePrimitives(unsigned _minNodePrimitives);


			/*
			 * @brief Procomputation of primitive's centroids and its morton codes
			 * @param _start - iterator to first primitive
			 * @param _end - iterator to last primitive
			 */
			void computeCenters(ge::sg::IndexedTriangleIterator& _start,
				ge::sg::IndexedTriangleIterator& _end);


			/*
			 * @brief Sort given geometry by primitives coordinaton
			 * @param _start - iterator to first primitive
			 * @param _end - iterator to last primitive
			 * @param first - iterator to global first primitive
			 * @param axis - axis used for sorting
			 */
			void sortCenters(ge::sg::IndexedTriangleIterator& _start,
				ge::sg::IndexedTriangleIterator& _end,
				ge::sg::IndexedTriangleIterator& first,
				DivideAxis axis);


			/*
			 * @brief Sort given geometry by primitives coordination
			 * @param _start - iterator to first primitive
			 * @param _end - iterator to last primitive
			 * @param first - iterator to global first primitive
			 * @param axis - axis used for sorting
			 */
			void sortCentersIndexed(ge::sg::IndexedTriangleIterator& _start,
				ge::sg::IndexedTriangleIterator& _end,
				ge::sg::IndexedTriangleIterator& first,
				DivideAxis axis);

		};

	}
}