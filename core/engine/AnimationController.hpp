#include <iostream>

namespace engine {


	static void update_animation(framework::Animation& animation, std::unordered_map<std::string, framework::SkeletonNode>& nodes) {

		using namespace framework;
		for (auto& channel : animation.channels)
		{
			const AnimationSampler& sampler = animation.samplers[channel.sampler_index];
			for (size_t i = 0; i < sampler.inputs.size(); i++)
			{
				float input0 = sampler.inputs[i];
				float input1 = 0.0f;

				glm::vec4 output0 = sampler.outputs[i];
				glm::vec4 output1{};
				if (i >= sampler.inputs.size()) {
					input1 = sampler.inputs[0];
					output1 = sampler.outputs[0];
				}
				else {
					input1 = sampler.inputs[i + 1];
					output1 = sampler.outputs[i + 1];
				}

				float a = (animation.current_time - input0) / (input1 - input0);

				if ((animation.current_time >= input0) && (animation.current_time <= input1))
				{
					assert(a <= 1.0f && a >= 0);
					glm::mat4 translation = glm::mat4(1.0f), scale = glm::mat4(1.0f), rotation = glm::mat4(1.0f);

					if (channel.property == ANIMATION_PROPERTY::TRANSLATION) {
						if (sampler.interpolation == ANIMATION_INTERPOLATION::LINEAR) {
							nodes[channel.target_node_name].translation = glm::translate(glm::mat4(1.0f), glm::mix(glm::vec3(output0), glm::vec3(output1), a));
						}
						else if (sampler.interpolation == ANIMATION_INTERPOLATION::STEP) {
							nodes[channel.target_node_name].translation = glm::translate(glm::mat4(1.0f), glm::vec3(output0));
						}
					}
					if (channel.property == ANIMATION_PROPERTY::SCALE)
						if (sampler.interpolation == ANIMATION_INTERPOLATION::LINEAR) {
							nodes[channel.target_node_name].scale = glm::scale(glm::mat4(1.0f), glm::mix(glm::vec3(output0), glm::vec3(output1), a));
						}
						else if (sampler.interpolation == ANIMATION_INTERPOLATION::STEP) {
							nodes[channel.target_node_name].scale = glm::scale(glm::mat4(1.0f), glm::vec3(output0));
						}

					if (channel.property == ANIMATION_PROPERTY::ROTATION) {
						glm::quat q0 = glm::quat(output0.x, output0.y, output0.z, output0.w);
						if (sampler.interpolation == ANIMATION_INTERPOLATION::LINEAR) {
							glm::quat q1 = glm::quat(output1.x, output1.y, output1.z, output1.w);
							nodes[channel.target_node_name].rotation = glm::mat4_cast(glm::normalize(glm::slerp(q0, q1, a)));
						}
						else if (sampler.interpolation == ANIMATION_INTERPOLATION::STEP) {
							nodes[channel.target_node_name].rotation = glm::mat4_cast(glm::normalize(q0));
						}
					}
				}
			}
		}
	}

	static void read_hierarchy(std::vector<glm::mat4>& outputMatrices,
		framework::SkeletonNode& node,
		glm::mat4 parent_transform,
		float time,
		framework::Skeleton& skeleton,
		const std::vector<framework::Animation>& animations) -> void {

		using namespace framework;

		glm::mat4 node_transform = node.transform;
		const Animation& animation = animations.back();
		int node_array_index = 0;

		if (skeleton.Joints.contains(node.name)) {
			node_array_index = skeleton.Joints[node.name].index;
			node_transform = skeleton.Nodes[node.name].getMatrix();
		}

		glm::mat4 global_transform = parent_transform * node_transform;

		if (skeleton.Joints.contains(node.name)) {
			outputMatrices[node_array_index] = global_transform * skeleton.inverse_matrices.at(node_array_index);
		}

		for (auto& child : node.children) {
			read_hierarchy(outputMatrices, skeleton.Nodes.at(child), global_transform, time, skeleton, animations);
		}

	}

	static void animate(std::vector<glm::mat4>& jointTransforms, std::shared_ptr<engine::Node<engine::RenderModel>> render_model, float time) {

		auto& animations = render_model->data.animations;
		auto& skeleton = render_model->data.skeleton;

		int index = 0;
		for (auto& animation : animations) {
			if (index == skeleton.activeAnimation) {
				if (animation.isActive)
					animation.current_time += framework::Clock::DeltaSeconds();

				if (animation.current_time > animation.end) {
					animation.current_time = animation.start;
				}


				glm::mat4 identity = glm::mat4(1.0f);
				update_animation(animation, skeleton.Nodes);
				read_hierarchy(jointTransforms, skeleton.Nodes.at(skeleton.root), identity, time, skeleton, animations);
			}
			index++;
		}

	}
}