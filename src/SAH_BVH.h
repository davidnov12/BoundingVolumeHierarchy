#pragma once

#include <geSG/MeshTriangleIterators.h>
#include <geSG/MeshPrimitiveIterator.h>

#include <iostream>
#include <functional>
#include <algorithm>
#include <chrono>
#include <valarray>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <BVH_Node.h>

namespace ge {
	namespace sg {

		/*
		* Implementation of common SAH BVH operations, declaring attributes
		*/
		class SAH_BVH {

		friend class ge::sg::IndexedTriangleIterator;

		public:

			SAH_BVH() {  }

			idlist(DivideAxis, X_AXIS, Y_AXIS, Z_AXIS);
			//idlist(SplitWay, OBJECT, SPATIAL);

			// Centroids structure
			typedef struct {
				unsigned trIndex;
				glm::vec3 center;
				unsigned mortonCode;
			} primitiveCenter;

			// Common attributes
			unsigned maxDepth;
			float binLength;
			bool firstPass = true;
			std::vector<primitiveCenter> associatedCenters;

		protected:

			/*
			 * Procomputation of primitive's centroids and its morton codes
			 * _start - iterator to first primitive
			 * _end - iterator to last primitive
			 */
			void computeCenters(ge::sg::IndexedTriangleIterator& _start,
								ge::sg::IndexedTriangleIterator& _end);


			/*
			 * Sort of given geometry by primitives morton codes
			 * _start - iterator to first primitive
			 * _end - iterator to last primitive
			 * first - iterator to global first primitive
			 * axis - axis used for sorting
			 */
			void sortCenters(ge::sg::IndexedTriangleIterator& _start,
							 ge::sg::IndexedTriangleIterator& _end,
							 ge::sg::IndexedTriangleIterator& first,
							 DivideAxis axis);

			void sortCentersIndexed(ge::sg::IndexedTriangleIterator& _start,
									ge::sg::IndexedTriangleIterator& _end,
									ge::sg::IndexedTriangleIterator& first,
									DivideAxis axis);

		private:

		};

	}
}