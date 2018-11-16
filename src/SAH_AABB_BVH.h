#pragma once

#include <SAH_BVH.h>

#include <geSG/AABB.h>

namespace ge {
	namespace sg {

		class SAH_AABB_BVH : SAH_BVH {
		
		public:

			using BVHNode = ge::sg::BVH_Node<ge::sg::AABB>;

			/*
			* _start - first primitive
			* _end - last primitive
			* _depth - maximum depth of BVH
			* _binLength - length of bin (uses SAH division), default is 0.2
			*/
			SAH_AABB_BVH(ge::sg::TriangleIterator& _start,
						 ge::sg::TriangleIterator& _end,
						 unsigned _depth,
						 float _binLength = 0.2f);

			/* ...
			*/
			SAH_AABB_BVH(ge::sg::Mesh& _geometry,
					     unsigned _depth,
						 float _binLength = 0.2f);

			/*
			* Function, which start build hierarchy
			* _start - first primitive
			* _end - last primitive
			*/
			void build(ge::sg::TriangleIterator& _start,
					   ge::sg::TriangleIterator& _end);

			/*
			* Returns pointer to root node of BVH
			*/
			std::shared_ptr<BVHNode> getRoot();

		protected:

		private:
		
			// Root node of BVH
			std::shared_ptr<BVHNode> rootNode;

			/*
			* Function, which recursively builds BVH structure
			* node - expanded node
			* currentDepth - depth of current node
			* axis - axis where is division performed
			*/
			void recursiveBuild(BVHNode& node,
								ge::sg::TriangleIterator& start,
								unsigned currentDepth,
								DivideAxis axis);

			/*
			* Searching for best divide position by SAH
			* node - divided node
			* start - position of first triangle
			* axis - axis where is division performed
			*/
			ge::sg::TriangleIterator divideBySAH(BVHNode& node,
												 ge::sg::TriangleIterator& start,
												 DivideAxis axis);

			/*
			* Evaluation of SAH for certain divide position
			* node - divided node
			* start - first triangle
			* result - computed SAH
			* criteria - divide position
			* divSize - size of divided subpart
			* axis - axis where is division performed
			*/
			ge::sg::TriangleIterator evaluateSAH(BVHNode& node,
												 ge::sg::TriangleIterator& start,
												 float& result,
												 float criteria,
												 float divSize,
												 DivideAxis axis);

		};

	}
}
