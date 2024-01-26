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

namespace Editor {
	class Editor : public Epsilon::Epsilon {

		engine::Ref<engine::RenderPass> m_pDefaultRenderPass;
		engine::Ref<engine::RenderPass> m_pForwardRenderPass; 

		std::shared_ptr<BrushManager> m_pBrushManager;

		engine::Ref<engine::ComputeShader> m_pComputeShader;

		engine::Ref<engine::BindGroup> m_pForwardBindGroup;
		engine::Ref<engine::Mesh> m_pDefaultCube;
		std::shared_ptr<engine::Node<engine::RenderModel>> m_pSkybox;
		std::shared_ptr<engine::Node<engine::RenderModel>> m_pGridPlane;

		UI::MenuBar m_pMenuBar;
		UI::MainViewport m_pMainViewport;
		UI::Inspector m_pInspector;
		UI::Gizmo m_pGuizmo;
		UI::Toolbar m_pTooldbar;
		UI::UIObjectProperty m_pObjectProperty;
		UI::SceneNodes m_pSceneNodes;
		UI::MaterialEditor m_pMaterialEditor;
		UI::Assets m_pAssets;
		UI::PostProcess m_pPostProcess;

		bool just_resized = false;

		size_t m_pTAABindGroup0;
		size_t m_pTAABindGroup1;
		size_t m_pDefaultBindGroup;
		size_t m_pSkyBindGroup;
		size_t m_pGrassBindGroup;
		size_t m_pGridBindGroup;


		std::shared_ptr<utils::Camera> m_pCamera;
		ShaderData shaderData;

		bool m_pCameraWasInactive = true;
		bool m_pNavigation = false;
		glm::mat4 selected_matrix = glm::mat4(1.0);
		bool manipulate = false;

		dotnet::DotnetHost host;
		int selected_index = -1;

		bool m_pCurrentTAAPass = false;

		Renderpasses::TAARenderPasses m_pTAAPasses;


		engine::Scene::SceneEntity* selected_entity = nullptr;

		std::shared_ptr<engine::NodeBase> m_pSelectedNode = nullptr;
		std::shared_ptr<GraphicsHelper> m_pGraphicsHelper;
		std::shared_ptr<engine::AssetManager> m_pAssetManager;

		struct tiny_keyboard {
			tiny_keyboard() {
				std::memset(keys, 0, 1024 * sizeof(int));
			}

			void convert() {
				for (int i = 0; i < 1024; i++) {
					keys[i] = (int)framework::Input::KeyBoard::KEYS[i];
				}
			}
			int keys[1024];
		};

		void UpdateReferenceCallback(engine::Scene* scene, engine::Node<engine::Scene::SceneEntity>* scene_ptr, void* managed_ref) {
			auto node = scene->getNode(scene_ptr->Index());
			auto script = scene->getChild<EntityScript>(node);
			script->data.ManagedPtr = managed_ref;
		}

		void pLoadDotnet() {

			host.Load(L"modules\\dotnet\\EpsilonSharp\\bin\\x64\\Debug\\net8.0\\");//"assets\\scripts\\EpsilonSharp\\bin\\x64\\Debug\\net7.0\\");
			//host.Load(L"assets\\scripts\\Game\\bin\\x64\\Debug\\net8.0\\");//"assets\\scripts\\EpsilonSharp\\bin\\x64\\Debug\\net7.0\\");

			typedef void (*func_ptr)(engine::Node<engine::Scene::SceneEntity>*, int, Transform);
			typedef void (*func_ptr_2)(engine::Scene*, engine::Node<engine::Scene::SceneEntity>*, void*);

			host.assembly.LoadDelegate<void, void*, engine::Node<engine::Scene::SceneEntity>*>(L"setEntityTransform", L"setEntityTransformDelegate", L"Epsilon");
			host.assembly.LoadDelegate<void*, void*, void*, const char*>(L"CreateEntity", L"", L"Epsilon", true);
			host.assembly.LoadDelegate<void, void*, float>(L"UpdateEntity", L"", L"Epsilon", true);
			host.assembly.LoadDelegate<void, func_ptr>(L"registerSetTransform", L"registerSetTransformDelegate", L"Epsilon");
			host.assembly.LoadDelegate<const char*, void*>(L"getEntityFields", L"getEntityFieldsDelegate", L"Epsilon");
			host.assembly.LoadDelegate<void, engine::Scene*>(L"setScenePtr", L"setScenePtrDelegate", L"Epsilon");
			host.assembly.LoadDelegate<void, const char*, float>(L"SetProperty", L"SetPropertyDelegate", L"Epsilon", true);
			host.assembly.LoadDelegate<void, const char*, bool>(L"SetPropertyBool", L"SetPropertyBoolDelegate", L"Epsilon", true);
			host.assembly.LoadDelegate<void>(L"ReloadAssemblies", L"ReloadAssembliesDelegate", L"Epsilon");
			host.assembly.LoadDelegate<void, func_ptr_2>(L"registerUpdateReferenceCallback", L"registerUpdateReferenceCallbackDelegate", L"Epsilon");

			host.assembly.LoadDelegate<void, tiny_keyboard>(L"setKeyboardState", L"setKeyboardStateDelegate", L"Epsilon");

			host.assembly.Invoke<void>(L"registerSetTransform", (func_ptr)[](engine::Node<engine::Scene::SceneEntity>* scene_ptr, int entity_id, Transform transform) {
				if (scene_ptr != nullptr) {
					scene_ptr->data.transform = transform.toMat4();
				}
				});

			host.assembly.Invoke<void>(L"registerUpdateReferenceCallback", (func_ptr_2)[](engine::Scene* scene, engine::Node<engine::Scene::SceneEntity>* scene_ptr, void* managed_ref) {
				
				if (scene == nullptr || scene_ptr == nullptr || managed_ref == nullptr) return;
				
				auto node = scene->getNode(scene_ptr->Index());
				auto script = scene->getChild<EntityScript>(node);
				script->data.ManagedPtr = managed_ref;

			});

			m_pSceneNodes.addEntityCallback = [this]() {
				//pAddDefaultCube(glm::vec3(0.0f));
				auto node = Utils::CreateNode(glm::mat4(1.0f), m_pScene);
				m_pSceneNodes.RegisterIntoEditor("Node_" + std::to_string(node->Index()), node);
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

			m_pAssetManager->Destroy();
			getContext()->CleanUp();
		}

		float range = glm::sin((float)framework::Clock::DeltaSeconds() * 0.02);

		void OnUpdate();

		void OnCreate();

		void pAddDefaultCube(glm::vec3 position);
		void pAddDefaultPlane(glm::vec3 position);
	};
}