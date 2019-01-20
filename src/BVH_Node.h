#pragma once

#include <memory>

#include <geSG/MeshPrimitiveIterator.h>
#include <geSG/MeshTriangleIterators.h>
#include <geSG/AABB.h>
#include <geSG/BoundingSphere.h>

namespace ge {
	namespace sg {
		
			template <typename Bound_volume> class BVH_Node {

			public:

			BVH_Node(Bound_volume vol,
				     ge::sg::IndexedTriangleIterator start,
				     ge::sg::IndexedTriangleIterator end) : volume(vol), first(start), last(end) {
			}

			BVH_Node(ge::sg::IndexedTriangleIterator start,
				ge::sg::IndexedTriangleIterator end) : first(start), last(end) {
			}

			~BVH_Node() {  }

			void setStartIterator(ge::sg::IndexedTriangleIterator start){
				first = start;
			}

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