#pragma once

#include <vector>
#include <unordered_map>

namespace framework {

	struct gltfAnimationNode {
		std::string name{};
		int index = -1;
		glm::mat4 inverseMatrix{};
		std::vector<gltfAnimationNode> children;
	};

	struct joint_sparse_container {
		std::vector<int> packed;
		std::vector<int> sparse;
	};

	struct gltfSkeleton {
		std::vector<glm::mat4> inverse_matrices;
		std::unordered_map<int, int> joint_indices;
		glm::mat4 globalMatrix{};
		gltfAnimationNode root;
	};

}