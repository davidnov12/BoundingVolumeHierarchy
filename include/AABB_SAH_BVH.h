/*
* RayTracing pro GPUEngine
* Diplomova prace - master's thesis
* Bc. David Novák
* FIT VUT Brno
* 2018/2019
*
* AABB_SAH_BVH.h
*/

#pragma once

#include <GeneralCPUBVH.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <geSG/AABB.h>

#include <BVH_Node.h>

#include <limits>
#include <chrono>

//#define CPU_BVH_MEASURE

namespace ge {
	namespace sg {

		/*
		* BVH structure using SAH building with AABB bounding volumes
		*/
		class AABB_SAH_BVH : public GeneralCPUBVH {

		public:
			
			// AABB node
			using BVHNode = ge::sg::BVH_Node<ge::sg::AABB>;

			/*
			* Function, which start hierarchy build
			*/
			void build() override;


			/*
			* Set number of dividing partitions
			*/
			void setSplitPartitions(unsigned numberOfParts);

			/*
			* Returns pointer to root node of BVH
			*/
			std::shared_ptr<BVHNode> getRoot();


			// Root node of BVH
			std::shared_ptr<BVHNode> rootNode;

			// number of candidate split planes
			unsigned nrOfPartitions = 10;

#ifdef CPU_BVH_MEASURE
			double sort, divide, minBB;
#endif

			/*
			* Function, which recursively builds BVH structure
			* node - expanded node
			* currentDepth - depth of current node
			* axis - axis where is division performed
			*/
			void recursiveBuild(BVHNode& node,
                                ge::sg::IndexedTriangleIterator& start,
                                unsigned currentDepth,
                                DivideAxis axis);

			/*
			* Searching for best divide position by SAH
			* node - divided node
			* start - position of first IndexedTriangle
			* axis - axis where is division performed
			*/
			ge::sg::IndexedTriangleIterator divideBySAH(BVHNode& node,
                                                        ge::sg::IndexedTriangleIterator& start,
                                                        DivideAxis axis);

			/*
			* Evaluation of SAH for certain divide position
			* node - divided node
			* start - first IndexedTriangle
			* result - computed SAH
			* criteria - divide position
			* divSize - size of divided subpart
			* axis - axis where is division performed
			*/
			ge::sg::IndexedTriangleIterator evaluateSAH(BVHNode& node,
                                                        ge::sg::IndexedTriangleIterator& start,
                                                        float& result,
                                                        float criteria,
                                                        float divSize, float boxSize,
                                                        DivideAxis axis);


		};

	}
}