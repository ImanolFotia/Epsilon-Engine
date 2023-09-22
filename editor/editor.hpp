#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <Epsilon.hpp>
#include <locale>
#include <codecvt>
#include "renderpasses/default.hpp"
#include "renderpasses/forward.hpp"

#include "ui/menu_bar.hpp"
#include "ui/main_viewport.hpp"
#include "ui/inspector.hpp"
#include "ui/gizmo.hpp"
#include "ui/toolbar.hpp"
#include "ui/object_property.hpp"
#include "ui/node_hierarchy.hpp"

#include "types/transform.hpp"
#include "utils/transform.hpp"

#include <core/engine/scene/scene.hpp>

#include <core/engine/renderer/drawables/primitives/sphere.hpp>
#include <core/engine/renderer/drawables/primitives/cube.hpp>
#include <core/engine/renderer/drawables/primitives/quad.hpp>
#include <core/framework/utils/helpers/camera.hpp>
#include <core/framework/IO/IO.hpp>


#include <modules/dotnet/src/host.hpp>

namespace Editor {
	class Editor : public Epsilon::Epsilon {

		engine::Ref<engine::RenderPass> m_pDefaultRenderPass;
		engine::Ref<engine::RenderPass> m_pForwardRenderPass;


		engine::Ref<engine::BindGroup> m_pForwardBindGroup;
		engine::Ref<engine::Mesh> m_pDefaultCube;

		UI::MenuBar m_pMenuBar;
		UI::MainViewport m_pMainViewport;
		UI::Inspector m_pInspector;
		UI::Gizmo m_pGuizmo;
		UI::Toolbar m_pTooldbar;
		UI::UIObjectProperty m_pObjectProperty;
		UI::SceneNodes m_pSceneNodes;

		std::shared_ptr<utils::Camera> m_pCamera;
		ShaderData shaderData;

		engine::Scene m_pScene;

		bool m_pCameraWasInactive = true;
		bool m_pNavigation = false;
		glm::mat4 selected_matrix = glm::mat4(1.0);
		bool manipulate = false;

		dotnet::DotnetHost host;
		int selected_index = -1;

		struct EntityArgs
		{
			const char_t* objectType = L"Game.GameObject";
			Transform transform;
			int id;
		};

		engine::Scene::SceneEntity* selected_entity = nullptr;

		std::shared_ptr<engine::NodeBase> m_pSelectedNode = nullptr;

		void pLoadDotnet() {

			host.Load(L"modules\\dotnet\\EpsilonSharp\\bin\\x64\\Debug\\net8.0\\");//"assets\\scripts\\EpsilonSharp\\bin\\x64\\Debug\\net7.0\\");

			typedef void (*func_ptr)(engine::Node<engine::Scene::SceneEntity>*, int, Transform);

			host.assembly.LoadDelegate<void, void*, engine::Node<engine::Scene::SceneEntity>*>(L"setEntityTransform", L"setEntityTransformDelegate", L"Epsilon");
			host.assembly.LoadDelegate<void*, void*, void*, const char*>(L"CreateEntity", L"", L"Epsilon", true);
			host.assembly.LoadDelegate<void, void*, float>(L"UpdateEntity", L"", L"Epsilon", true);
			host.assembly.LoadDelegate<void, func_ptr>(L"registerSetTransform", L"registerSetTransformDelegate", L"Epsilon");
			host.assembly.LoadDelegate<const char*, void*>(L"getEntityFields", L"getEntityFieldsDelegate", L"Epsilon");
			host.assembly.LoadDelegate<void, engine::Scene*>(L"setScenePtr", L"setScenePtrDelegate", L"Epsilon");
			host.assembly.LoadDelegate<void, const char*, float>(L"SetProperty", L"SetPropertyDelegate", L"Epsilon", true);
			host.assembly.LoadDelegate<void, const char*, bool>(L"SetPropertyBool", L"SetPropertyBoolDelegate", L"Epsilon", true);

			host.assembly.Invoke<void>(L"registerSetTransform", (func_ptr)[](engine::Node<engine::Scene::SceneEntity>* scene_ptr, int entity_id, Transform transform) {
				if (scene_ptr != nullptr) {
					scene_ptr->data.transform = transform.toMat4();
				}
				});

			m_pSceneNodes.addEntityCallback = [this]() {
				pAddDefaultCube(glm::vec3(0.0f));
				};
		}
	public:
		Editor(const std::string name) : Epsilon::Epsilon(name) {
			this->setOnCreate([this] { OnCreate(); });
			this->setOnRender([this] { OnRender(); });
			this->setOnExit([this] { OnExit(); });
			this->setOnUpdate([this] {OnUpdate(); });

			pLoadDotnet();
		}

		void OnRender();

		void OnExit() {
			getContext()->CleanUp();
		}

		float range = glm::sin((float)framework::Clock::DeltaSeconds() * 0.02);

		void OnUpdate();

		void OnCreate();

		void pAddDefaultCube(glm::vec3 position);
	};
}