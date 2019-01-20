#pragma once
 
#include <geSG/MeshTriangleIterators.h>
#include <geSG/MeshPrimitiveIterator.h>

#include <algorithm>
#include <vector>

namespace ge {
	namespace sg {

		/*
	 	 * Class with common attributes and functions for BVH build
		 */
		class BVHBuildPolicy {

		protected:

			// Enumeration of dividing axises
			idlist(DivideAxis, X_AXIS, Y_AXIS, Z_AXIS);

			// Build function
			virtual void build();

			// Centroids structure
			typedef struct {
				unsigned trIndex;
				glm::vec3 center;
				unsigned mortonCode;
			} primitiveCenter;

			// Common attributes
			unsigned maxDepth = 10;
			unsigned dividePartitions = 5;
			unsigned minVolumePrimitives = 10;
			bool firstPass = true;
			std::vector<primitiveCenter> associatedCenters;
			ge::sg::IndexedTriangleIterator _firstPrimitive, _lastPrimitive;


			/*
			* _start - first primitive
			* _end - last primitive
			*/
			void setGeometry(ge::sg::IndexedTriangleIterator& _start,
				ge::sg::IndexedTriangleIterator& _end);


			/*
			* _geometry - geometry for BVH
			*/
			void setGeometry(ge::sg::Mesh& _geometry);


			/*
			* _depth - maximum depth of BVH
			*/
			void setMaxDepth(unsigned _depth);


			/*
			* _minNodePrimitives - minimum number of primitives in node
			*/
			void setMinNodePrimitives(unsigned _minNodePrimitives);


			/*
			 * Procomputation of primitive's centroids and its morton codes
			 * _start - iterator to first primitive
			 * _end - iterator to last primitive
			 */
			void computeCenters(ge::sg::IndexedTriangleIterator& _start,
				ge::sg::IndexedTriangleIterator& _end);


			/*
			 * Sort given geometry by primitives coordinaton
			 * _start - iterator to first primitive
			 * _end - iterator to last primitive
			 * first - iterator to global first primitive
			 * axis - axis used for sorting
			 */
			void sortCenters(ge::sg::IndexedTriangleIterator& _start,
				ge::sg::IndexedTriangleIterator& _end,
				ge::sg::IndexedTriangleIterator& first,
				DivideAxis axis);


			/*
			 * Sort given geometry by primitives coordination
			 * _start - iterator to first primitive
			 * _end - iterator to last primitive
			 * first - iterator to global first primitive
			 * axis - axis used for sorting
			 */
			void sortCentersIndexed(ge::sg::IndexedTriangleIterator& _start,
				ge::sg::IndexedTriangleIterator& _end,
				ge::sg::IndexedTriangleIterator& first,
				DivideAxis axis);

		};

	}
}