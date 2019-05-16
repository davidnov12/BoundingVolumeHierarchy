/*
* RayTracing pro GPUEngine
* Diplomova prace - master's thesis
* Bc. David Novák
* FIT VUT Brno
* 2018/2019
*
* BVH_Node.h
*/

#pragma once

#include <memory>

#include <geSG/MeshPrimitiveIterator.h>
#include <geSG/MeshTriangleIterators.h>
#include <geSG/AABB.h>
#include <geSG/BoundingSphere.h>

namespace ge {
	namespace sg {
		
			/*
			* @brief BVH node, which maintains data in BVH node (childs, primitives, bounding volume informations)
			* @note typename Bound_volume - expects class inherited from class ge::sg::BoundingVolume
			*/
			template <typename Bound_volume> class BVH_Node {

			public:

			/*
			* @brief Constructor, set up initial values
			* @param vol bounding volume of node
			* @param start first primitive iterator
			* @param end last primitive iterator
			*/
			BVH_Node(Bound_volume vol,
				     ge::sg::IndexedTriangleIterator start,
				     ge::sg::IndexedTriangleIterator end) : volume(vol), first(start), last(end) {
			}

			/*
			/*
			* @brief Constructor, set up initial values
			* @param start first primitive iterator
			* @param end last primitive iterator
			*/
			BVH_Node(ge::sg::IndexedTriangleIterator start,
				ge::sg::IndexedTriangleIterator end) : first(start), last(end) {
			}

			/*
			* @breif Destructor
			*/
			~BVH_Node() {  }

			/*
			* @brief Set new start iterator
			* @param start new start iterator
			*/
			void setStartIterator(ge::sg::IndexedTriangleIterator start){
				first = start;
			}

			/*
			* @brief Set new end iterator
			* @param end new end iterator
			*/
			void setEndIterator(ge::sg::IndexedTriangleIterator end) {
				last = end;
			}

			Bound_volume volume;							// Bounding Volume
			ge::sg::IndexedTriangleIterator first, last;	// Primitives in node
			std::shared_ptr<BVH_Node> left, right;			// Pointers to childs

		protected:

		private:


		};
		
	}
}