#pragma once

#include <vector>
#include <unordered_map>

namespace framework {

	enum class ANIMATION_PROPERTY {
		TRANSLATION = 0x001,
		ROTATION = 0x010,
		SCALE = 0x100
	};

	enum class ANIMATION_INTERPOLATION {
		LINEAR = 0x0001,
		STEP = 0x0010,
		CATMULLROMSPLINE = 0x0100,
		CUBICSPLINE = 0x1000
	};



	struct AnimationSampler {
		std::vector<float> inputs;
		std::vector<glm::vec4> outputs;
		ANIMATION_INTERPOLATION interpolation;
	};

	struct AnimationChannel {
		ANIMATION_PROPERTY property;
		int target_node{};
		std::string target_node_name{};
		int sampler_index{};
	};

	struct Animation {
		std::string name{};
		float duration = 0.0f;
		float start = 0.0f;
		float end = 0.0f;
		std::vector<AnimationSampler> samplers;
		std::vector<AnimationChannel> channels;
		float current_time = 0.0f;
	};

	struct SkeletonNode {
		std::string name{};
		int index = -1;
		glm::mat4 transform = glm::mat4(1.0f);
		glm::mat4 inverseMatrix{};
		std::vector<std::string> children;
		glm::mat4 node_matrix = glm::mat4(1.0f);
		glm::mat4 translation = glm::mat4(1.0f);
		glm::mat4 rotation = glm::mat4(1.0f);
		glm::mat4 scale = glm::mat4(1.0f);

		bool hasPosition = false;
		bool hasScale = false;
		bool hasRotation = false;

		glm::mat4 getMatrix() {
			return (translation * rotation * scale);
		}
	};

	struct Skeleton {
		std::vector<glm::mat4> inverse_matrices;
		glm::mat4 globalTransform{};
		glm::mat4 globalInverseTransform{};
		std::string root;
		std::unordered_map<std::string, SkeletonNode> Joints;
		std::unordered_map<std::string, SkeletonNode> Nodes;
	};
}