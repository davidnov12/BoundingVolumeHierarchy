#pragma once

#include <BVH_Node.h>

class BVH_View {

public:

	BVH_View(ge::sg::BVH_Node<ge::sg::AABB> root) {
		generate(root);
	}
	
	float* getBVHGeometry() {
		return (float*)bvh.data();
	}

protected:

private:

	void generate(ge::sg::BVH_Node<ge::sg::AABB> root) {

		if (root.left != nullptr) generate(*root.left);
		if (root.right != nullptr) generate(*root.left);
		
		if (root.left == nullptr || root.right == nullptr) {

			bvh.push_back(glm::vec3(root.volume.min));
			bvh.push_back(glm::vec3(root.volume.min.x, root.volume.max.y, root.volume.min.z));

			bvh.push_back(glm::vec3(root.volume.min));
			bvh.push_back(glm::vec3(root.volume.max.x, root.volume.min.y, root.volume.min.z));

			bvh.push_back(glm::vec3(root.volume.min.x, root.volume.max.y, root.volume.min.z));
			bvh.push_back(glm::vec3(root.volume.max.x, root.volume.max.y, root.volume.min.z));

			bvh.push_back(glm::vec3(root.volume.max.x, root.volume.min.y, root.volume.min.z));
			bvh.push_back(glm::vec3(root.volume.max.x, root.volume.max.y, root.volume.min.z));

			bvh.push_back(glm::vec3(root.volume.min));
			bvh.push_back(glm::vec3(root.volume.min.x, root.volume.min.y, root.volume.max.z));

			bvh.push_back(glm::vec3(root.volume.max.x, root.volume.max.y, root.volume.min.z));
			bvh.push_back(glm::vec3(root.volume.max));

			bvh.push_back(glm::vec3(root.volume.min.x, root.volume.max.y, root.volume.min.z));
			bvh.push_back(glm::vec3(root.volume.min.x, root.volume.max.y, root.volume.max.z));

			bvh.push_back(glm::vec3(root.volume.max.x, root.volume.min.y, root.volume.min.z));
			bvh.push_back(glm::vec3(root.volume.max.x, root.volume.min.y, root.volume.max.z));

			bvh.push_back(glm::vec3(root.volume.min.x, root.volume.min.y, root.volume.max.z));
			bvh.push_back(glm::vec3(root.volume.min.x, root.volume.max.y, root.volume.max.z));

			bvh.push_back(glm::vec3(root.volume.min.x, root.volume.min.y, root.volume.max.z));
			bvh.push_back(glm::vec3(root.volume.max.x, root.volume.min.y, root.volume.max.z));

			bvh.push_back(glm::vec3(root.volume.min.x, root.volume.max.y, root.volume.max.z));
			bvh.push_back(glm::vec3(root.volume.max));

			bvh.push_back(glm::vec3(root.volume.max.x, root.volume.min.y, root.volume.max.z));
			bvh.push_back(glm::vec3(root.volume.max));

			return;
		}

	}

	std::vector<glm::vec3> bvh;

};