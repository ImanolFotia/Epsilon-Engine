#pragma once

#include <imgui.h>
#include <modules/imgui/custom_widgets.hpp>

#include <memory>

namespace Editor::UI
{
	class Inspector : public UIElement
	{

		struct SelectedTransform
		{
			glm::vec3 position{};
			glm::vec3 scale{};
			glm::quat rotation{};
		};

		SelectedTransform m_pSelectedTransform;

	public:
	private:
		TransformationMode m_pMode = TRANSLATE;
		bool m_pBoundScaling = false;
		std::string m_pName{};
		bool m_pLockScaleAxis = true;

	public:
		void setName(const std::string name)
		{
			m_pName = name;
		}

		void draw() override
		{
			ImGui::Begin("Inspector");
			ImGui::InputText("Name", m_pName.data(), m_pName.size() + 1);
			ImGui::Separator();
			ImGui::Text("Transform");

			EpsilonUI::vector3("Position", &m_pSelectedTransform.position);

			ImGui::Checkbox("Lock Scale Axis", &m_pLockScaleAxis);
			if (m_pLockScaleAxis)
			{
				float *scale = &m_pSelectedTransform.scale.x;
				if (ImGui::DragFloat("Scale##lock_scale", scale, 0.001f))
				{
					m_pSelectedTransform.scale = glm::vec3(m_pSelectedTransform.scale.x);
				}
			}
			else
			{

				EpsilonUI::vector3("Scale", &m_pSelectedTransform.scale);
			}
			glm::vec3 rotation = glm::eulerAngles(m_pSelectedTransform.rotation);
			EpsilonUI::vector3("Rotation", &rotation);
			m_pSelectedTransform.rotation = glm::tquat<float>(rotation);
			ImGui::End();
		}

		void setTransform(glm::mat4 transform)
		{
			glm::vec3 skew = glm::vec3(1.0f);
			glm::vec4 proj = glm::vec4(1.0f);
			glm::decompose(transform, m_pSelectedTransform.scale, m_pSelectedTransform.rotation, m_pSelectedTransform.position, skew, proj);
		}

		glm::mat4 getTransform()
		{
			glm::mat4 model_transform = glm::mat4(1.0f);
			model_transform = glm::translate(glm::mat4(1.0f), m_pSelectedTransform.position);
			model_transform *= glm::toMat4(m_pSelectedTransform.rotation);
			model_transform *= glm::scale(glm::mat4(1.0f), m_pSelectedTransform.scale);
			return model_transform;
		}

		TransformationMode getMode()
		{
			return m_pMode;
		}

		bool boundScaling()
		{
			return m_pBoundScaling;
		}
	};
}