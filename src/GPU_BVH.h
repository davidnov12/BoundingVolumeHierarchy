#pragma once
#include <SAH_BVH.h>
#include <geGL/geGL.h>
#include <geGL/StaticCalls.h>

namespace ge {
	namespace sg {

		class GPU_BVH {

		public:

			GPU_BVH(ge::sg::IndexedTriangleIterator _start, ge::sg::IndexedTriangleIterator _end);
			GPU_BVH(ge::sg::Mesh _geometry);

		protected:

		private:

			ge::gl::Program mortonCode, radixSort, radixTree;
			ge::gl::Buffer geomIn, geomOut, mortonIn, mortonOut, nodes;

		};

	}
}