#pragma once

#include <memory>

#include <geSG/MeshPrimitiveIterator.h>
#include <geSG/MeshTriangleIterators.h>
#include <geSG/AABB.h>

namespace ge {
	namespace sg {
		
			template <typename _bound_volume> class BVH_Node {

			public:

			BVH_Node(_bound_volume vol,
				     ge::sg::TriangleIterator start,
				     ge::sg::TriangleIterator end) : volume(vol), first(start), last(end) {
			}

			BVH_Node(ge::sg::TriangleIterator start,
				ge::sg::TriangleIterator end) : first(start), last(end) {
			}

			~BVH_Node() {  }

			void setStartIterator(ge::sg::TriangleIterator start){
				first = start;
			}

			void setEndIterator(ge::sg::TriangleIterator end) {
				last = end;
			}

			_bound_volume volume;							// Bounding Volume
			ge::sg::TriangleIterator first, last;	// Primitives in node
			std::shared_ptr<BVH_Node> left, right;			// Pointers to childs

		protected:

		private:


		};
		
	}
}