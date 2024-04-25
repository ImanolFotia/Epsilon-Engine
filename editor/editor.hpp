#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <Epsilon.hpp>
#include <locale>
#include <codecvt>
#include "renderpasses/default.hpp"
#include "renderpasses/forward.hpp"
#include "renderpasses/compute.hpp"
#include "renderpasses/taa.hpp"

#include "ui/menu_bar.hpp"
#include "ui/main_viewport.hpp"
#include "ui/inspector.hpp"
#include "ui/gizmo.hpp"
#include "ui/toolbar.hpp"
#include "ui/object_property.hpp"
#include "ui/node_hierarchy.hpp"
#include "ui/material_editor.hpp"
#include "ui/assets.hpp"
#include "ui/postprocess.hpp"

#include "types/transform.hpp"
#include "utils/transform.hpp"

#include <core/engine/scene/scene.hpp>

#include <core/engine/renderer/drawables/primitives/sphere.hpp>
#include <core/engine/renderer/drawables/primitives/sphere2.hpp>
#include <core/engine/renderer/drawables/primitives/cube.hpp>
#include <core/engine/renderer/drawables/primitives/quad.hpp>
#include <core/engine/renderer/drawables/primitives/cylinder.hpp>
#include <core/framework/utils/helpers/camera.hpp>
#include <core/framework/IO/IO.hpp>

#include "brushes/brush_manager.hpp"

#include <modules/dotnet/src/host.hpp>
#include "types/script.hpp"
#include "utils/node_factory.hpp"

#include "renderpasses/setup_graphics.hpp"

namespace Editor
{
	class Editor : public Epsilon::Epsilon
	{

		engine::Ref<engine::RenderPass> m_DefaultRenderPass;
		engine::Ref<engine::RenderPass> m_ForwardRenderPass;

		std::shared_ptr<BrushManager> m_BrushManager;

		engine::Ref<engine::ComputeShader> m_ComputeShader;

		engine::Ref<engine::BindGroup> m_ForwardBindGroup;
		engine::Ref<engine::Mesh> m_DefaultCube;
		std::shared_ptr<engine::Node<engine::RenderModel>> m_Skybox;
		std::shared_ptr<engine::Node<engine::RenderModel>> m_GridPlane;

		UI::MenuBar m_MenuBar;
		UI::MainViewport m_MainViewport;
		UI::Inspector m_Inspector;
		UI::Gizmo m_Guizmo;
		UI::Toolbar m_Tooldbar;
		UI::UIObjectProperty m_ObjectProperty;
		UI::SceneNodes m_SceneNodes;
		UI::MaterialEditor m_MaterialEditor;
		UI::Assets m_Assets;
		UI::PostProcess m_PostProcess;

		bool just_resized = false;

		size_t m_TAABindGroup0;
		size_t m_TAABindGroup1;
		size_t m_DefaultBindGroup;
		size_t m_SkyBindGroup;
		size_t m_GrassBindGroup;
		size_t m_GridBindGroup;

		std::shared_ptr<utils::Camera> m_Camera;
		ShaderData shaderData;

		bool m_CameraWasInactive = true;
		bool m_Navigation = false;
		glm::mat4 selected_matrix = glm::mat4(1.0);
		bool manipulate = false;

		dotnet::DotnetHost host;
		int selected_index = -1;

		bool m_CurrentTAAPass = false;

		Renderpasses::TAARenderPasses m_TAAPasses;

		engine::Scene::SceneEntity *selected_entity = nullptr;

		std::shared_ptr<engine::NodeBase> m_SelectedNode = nullptr;
		std::shared_ptr<GraphicsHelper> m_GraphicsHelper;
		std::shared_ptr<engine::AssetManager> m_AssetManager;

		struct tiny_keyboard
		{
			tiny_keyboard()
			{
				std::memset(keys, 0, 1024 * sizeof(int));
			}

			void convert()
			{
				for (int i = 0; i < 1024; i++)
				{
					keys[i] = (int)framework::Input::KeyBoard::KEYS[i];
				}
			}
			int keys[1024];
		};

		void UpdateReferenceCallback(engine::Scene *scene, engine::Node<engine::Scene::SceneEntity> *scene_ptr, void *managed_ref)
		{
			auto node = scene->getNode(scene_ptr->Index());
			auto script = scene->getChild<EntityScript>(node);
			script->data.ManagedPtr = managed_ref;
		}

		void pLoadDotnet()
		{

			host.Load(L"./modules/dotnet/EpsilonSharp/bin/x64/Debug/net8.0");
			// host.Load(L"modules\\dotnet\\EpsilonSharp\\bin\\x64\\Debug\\net8.0\\"); //"assets\\scripts\\EpsilonSharp\\bin\\x64\\Debug\\net7.0\\");
			//  host.Load(L"assets\\scripts\\Game\\bin\\x64\\Debug\\net8.0\\");//"assets\\scripts\\EpsilonSharp\\bin\\x64\\Debug\\net7.0\\");

			typedef void (*func_ptr)(engine::Node<engine::Scene::SceneEntity> *, int, Transform);
			typedef void (*func_ptr_2)(engine::Scene *, engine::Node<engine::Scene::SceneEntity> *, void *);

			host.assembly.LoadDelegate<void, void *, engine::Node<engine::Scene::SceneEntity> *>(STR("setEntityTransform"), STR("setEntityTransformDelegate"), STR("Epsilon"));
			host.assembly.LoadDelegate<void *, void *, void *, const char *>(STR("CreateEntity"), STR(""), STR("Epsilon"), true);
			host.assembly.LoadDelegate<void, void *, float>(STR("UpdateEntity"), STR(""), STR("Epsilon"), true);
			host.assembly.LoadDelegate<void, func_ptr>(STR("registerSetTransform"), STR("registerSetTransformDelegate"), STR("Epsilon"));
			host.assembly.LoadDelegate<const char *, void *>(STR("getEntityFields"), STR("getEntityFieldsDelegate"), STR("Epsilon"));
			host.assembly.LoadDelegate<void, engine::Scene *>(STR("setScenePtr"), STR("setScenePtrDelegate"), STR("Epsilon"));
			host.assembly.LoadDelegate<void, const char *, float>(STR("SetProperty"), STR("SetPropertyDelegate"), STR("Epsilon"), true);
			host.assembly.LoadDelegate<void, const char *, bool>(STR("SetPropertyBool"), STR("SetPropertyBoolDelegate"), STR("Epsilon"), true);
			host.assembly.LoadDelegate<void>(STR("ReloadAssemblies"), STR("ReloadAssembliesDelegate"), STR("Epsilon"));
			host.assembly.LoadDelegate<void, func_ptr_2>(STR("registerUpdateReferenceCallback"), STR("registerUpdateReferenceCallbackDelegate"), STR("Epsilon"));

			host.assembly.LoadDelegate<void, tiny_keyboard>(STR("setKeyboardState"), STR("setKeyboardStateDelegate"), STR("Epsilon"));

			host.assembly.Invoke<void>(L"registerSetTransform", (func_ptr)[](engine::Node<engine::Scene::SceneEntity> * scene_ptr, int entity_id, Transform transform) {
				if (scene_ptr != nullptr)
				{
					scene_ptr->data.transform = transform.toMat4();
				} });

			host.assembly.Invoke<void>(L"registerUpdateReferenceCallback", (func_ptr_2)[](engine::Scene * scene, engine::Node<engine::Scene::SceneEntity> * scene_ptr, void *managed_ref) {
				if (scene == nullptr || scene_ptr == nullptr || managed_ref == nullptr)
					return;

				auto node = scene->getNode(scene_ptr->Index());
				auto script = scene->getChild<EntityScript>(node);
				script->data.ManagedPtr = managed_ref; });

			m_SceneNodes.addEntityCallback = [this]()
			{
				// pAddDefaultCube(glm::vec3(0.0f));
				auto node = Utils::CreateNode(glm::mat4(1.0f), m_Scene);
				m_SceneNodes.RegisterIntoEditor("Node_" + std::to_string(node->Index()), node);
			};
		}

	public:
		Editor(const std::string name) : Epsilon::Epsilon(name)
		{
			this->setOnCreate([this]
							  { OnCreate(); });
			this->setOnRender([this]
							  { OnRender(); });
			this->setOnExit([this]
							{ OnExit(); });
			this->setOnUpdate([this]
							  { OnUpdate(); });

			pLoadDotnet();
		}

		void OnRender();

		void OnExit()
		{

			m_AssetManager->Destroy();
			getContext()->CleanUp();
		}

		float range = glm::sin((float)framework::Clock::DeltaSeconds() * 0.02);

		void OnUpdate();

		void OnCreate();

		void pAddDefaultCube(glm::vec3 position);
		void pAddDefaultPlane(glm::vec3 position);
	};
}