#pragma once

#include <geSG/MeshTriangleIterators.h>
#include <geSG/MeshPrimitiveIterator.h>

#include <BVHBuildPolicy.h>

namespace ge {
	namespace sg {

		/*
		* General BVH class
		*/
		template <typename BuildPolicy, typename NodeType> class BVH : private BuildPolicy {

			using BuildPolicy::build;
			using BuildPolicy::setGeometry;
			using BuildPolicy::setMaxDepth;
			using BuildPolicy::setMinNodePrimitives;
			using BuildPolicy::rootNode;

		public:

			// Start BVH build
			void buildBVH(){
				build();
			}

			// Geometry data for BVH
			void setGeometryData(ge::sg::IndexedTriangleIterator& _start,
							     ge::sg::IndexedTriangleIterator& _end) {
				setGeometry(_start, _end);
			}

			// Geometry data for BVH
			void setGeometryData(ge::sg::Mesh& _mesh) {
				setGeometry(_mesh);
			}

			// Maximal depth of BVH
			void setDepth(unsigned depth) {
				setMaxDepth(depth);
			}

			// Minimal number of primitives in BVH node
			void setMinimumPrimitivesInNode(unsigned primitivesNumber) {
				setMinNodePrimitives(primitivesNumber);
			}

			// Root node of BVH
			std::shared_ptr<NodeType> getRootNode() {
				return rootNode;
			}

		};

	}
}