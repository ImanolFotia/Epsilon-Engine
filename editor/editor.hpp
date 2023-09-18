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

#include <core/engine/scene/scene.hpp>

#include <core/engine/renderer/drawables/primitives/sphere.hpp>
#include <core/engine/renderer/drawables/primitives/cube.hpp>
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

		struct Transform
		{
			float position[3] = { 1.0, 2.0, 3.0 };
			float rotation[4] = { 7.0, 8.0, 9.0, 1.0 };
			float scale[3] = { 4.0, 5.0, 6.0 };
		};


		struct EntityArgs
		{
			const char_t* objectType = L"Game.GameObject";
			Transform transform;
			int id;
		};

		engine::Scene::SceneEntity* selected_entity = nullptr;


		//std::shared_ptr<engine::Node<EntityScript>> m_pScript;
		std::shared_ptr<engine::NodeBase> m_pSelectedNode = nullptr;

		void loadDotnet() {

			host.Load(L"assets\\scripts\\editor\\bin\\x64\\Debug\\net7.0\\");

			typedef void (*func_ptr)(engine::Node<engine::Scene::SceneEntity>*, int, Transform);

			host.assembly.LoadDelegate<void, void*, engine::Node<engine::Scene::SceneEntity>*>(L"setEntityTransform", L"setEntityTransformDelegate", L"Epsilon");
			host.assembly.LoadDelegate<void*, void*, void*>(L"CreateEntity", L"", L"Epsilon", true);
			host.assembly.LoadDelegate<void, void*, float>(L"UpdateEntity", L"", L"Epsilon", true);
			host.assembly.LoadDelegate<void, func_ptr>(L"registerSetTransform", L"registerSetTransformDelegate", L"Epsilon");
			host.assembly.LoadDelegate<const char*, void*>(L"getEntityFields", L"getEntityFieldsDelegate", L"Epsilon");
			host.assembly.LoadDelegate<void, engine::Scene*>(L"setScenePtr", L"setScenePtrDelegate", L"Epsilon");
			host.assembly.LoadDelegate<void, const char*, float>(L"SetProperty", L"SetPropertyDelegate", L"Epsilon", true);
			host.assembly.LoadDelegate<void, const char*, bool>(L"SetPropertyBool", L"SetPropertyBoolDelegate", L"Epsilon", true);

			host.assembly.Invoke<void>(L"registerSetTransform", (func_ptr)[](engine::Node<engine::Scene::SceneEntity>* scene_ptr, int entity_id, Transform transform) {
				if (scene_ptr != nullptr) {
					scene_ptr->data.position.x = transform.position[0];
					scene_ptr->data.position.y = transform.position[1];
					scene_ptr->data.position.z = transform.position[2];

					scene_ptr->data.scale.x = transform.scale[0];
					scene_ptr->data.scale.y = transform.scale[1];
					scene_ptr->data.scale.z = transform.scale[2];
				}
				});
		}
	public:
		Editor(const std::string name) : Epsilon::Epsilon(name) {
			this->setOnCreate([this] { OnCreate(); });
			this->setOnRender([this] { OnRender(); });
			this->setOnExit([this] { OnExit(); });
			this->setOnUpdate([this] {OnUpdate(); });

			m_pMenuBar.addCallback("OnExit", [this]() { mShouldClose = true; });

			m_pCamera = std::make_shared<utils::Camera>(glm::vec3(5.0, 5.0, 4.5), glm::vec3(0.0) - glm::vec3(5.0, 5.0, 4.5));

			m_pInspector.setTransform(selected_matrix);
			m_pInspector.setName("Cube");

			loadDotnet();

		}

		void OnRender() {

			m_pScene.BeginScene();

			m_pScene.setCurrentRenderPass("ForwardRenderPass");


			auto models = m_pScene.getNodes <engine::RenderModel>();

			for (auto& model_node : models) {
				auto model = std::static_pointer_cast<engine::Node<engine::RenderModel>>(*model_node);
				auto transform = std::static_pointer_cast<engine::Node<engine::Scene::SceneEntity>>(model->Parent());

				glm::mat4 model_transform = glm::mat4(1.0f);
				model_transform = glm::translate(glm::mat4(1.0f), transform->data.position);
				model_transform *= glm::toMat4(transform->data.rotation);
				model_transform *= glm::scale(glm::mat4(1.0f), transform->data.scale);
				//selected_matrix = m_pInspector.getTransform();
				m_pScene.Push(model, model_transform, "DefaultBindGroup");
			}
			m_pScene.Flush();

			m_pScene.EndScene();

		}

		void OnExit() {
			getContext()->CleanUp();
		}

		float range = glm::sin((float)framework::Clock::DeltaSeconds() * 0.02);

		void OnUpdate() {

			auto models = m_pScene.getNodes <engine::RenderModel>();
			auto scripts = m_pScene.getNodes <EntityScript>();


			for (auto s : scripts) {
				auto script = std::static_pointer_cast<engine::Node<EntityScript>>(*s);
				auto transform = std::static_pointer_cast<engine::Node<engine::Scene::SceneEntity>>(script->Parent());
				Transform t;
				t.position[0] = transform->data.position.x;
				t.position[1] = transform->data.position.y;
				t.position[2] = transform->data.position.z;

				t.scale[0] = transform->data.scale.x;
				t.scale[1] = transform->data.scale.y;
				t.scale[2] = transform->data.scale.z;

				EntityArgs args;
				args.transform = t;
				args.id = 0;
				host.assembly.Invoke<void>(L"setEntityTransform", script->data.ManagedPtr, args);
				if (m_pTooldbar.getStatus() == UI::Toolbar::STATUS::Playing)
					host.assembly.Invoke<void>(L"UpdateEntity", script->data.ManagedPtr, (float)framework::Clock::DeltaSeconds());


			}



			if (selected_entity != nullptr) {

				glm::mat4 model_transform = glm::mat4(1.0f);
				model_transform = glm::translate(glm::mat4(1.0f), selected_entity->position);
				model_transform *= glm::toMat4(selected_entity->rotation);
				model_transform *= glm::scale(glm::mat4(1.0f), selected_entity->scale);

				//if (selected_matrix != model_transform) {
					//selected_matrix = model_transform;
					//m_pInspector.setTransform(model_transform);

				glm::vec3 a;
				glm::vec4 b;
				glm::decompose(model_transform, selected_entity->scale, selected_entity->rotation, selected_entity->position, a, b);
				selected_matrix = model_transform;
				m_pInspector.setTransform(selected_matrix);
				//}
			}

			int screenX, screenY; //getWindowDimensions();
			screenX = m_pMainViewport.getSize().x;
			screenY = m_pMainViewport.getSize().y;
			shaderData.iResolution = glm::vec2(screenX, screenY);

			range += 0.001f;
			range = glm::mod(range, 2.0f);


			m_pCamera->UpdateMatrices(0, screenX, screenY, false);

			if (framework::Input::Mouse::MIDDLE == framework::Input::PRESSED && m_pMainViewport.isHovered()) {
				if (m_pCameraWasInactive) {
					m_pCamera->ResetDeltas();
					m_pCameraWasInactive = false;
				}
				getContext()->Window().HideCursor();
				m_pCamera->Update(Epsilon::getContext()->Window().getWindow());
				m_pNavigation = true;
			}
			else {
				getContext()->Window().ShowCursor();
				m_pCameraWasInactive = true;
				m_pNavigation = false;
			}

			static auto startTime = std::chrono::high_resolution_clock::now();
			auto currentTime = std::chrono::high_resolution_clock::now();
			auto time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

			shaderData.view = m_pCamera->getViewMatrix();
			shaderData.proj = m_pCamera->getProjectionMatrix();
			shaderData.proj[1][1] *= -1;
			shaderData.iTime += time;
			shaderData.viewPosition = m_pCamera->getPosition();

			if (m_pMainViewport.ShouldResize()) {
				getContext()->ResourceManager()->ResizeFramebuffer(m_pForwardRenderPass, m_pMainViewport.getSize());
				Epsilon::getContext()->Renderer()->getDebugRenderer()->recreateDescriptorSets();
				m_pMainViewport.ResetFlags();
			}

			getContext()->Renderer()->UpdateRenderPassUniforms(m_pForwardRenderPass, engine::RENDERPASS_SET, &shaderData);

		}

		void OnCreate() {
			m_pScene = engine::Scene(getContext());

			host.assembly.Invoke<void>(L"setScenePtr", &m_pScene);

			m_pForwardRenderPass = Renderpasses::createForwardRenderPass(Epsilon::getContext());

			m_pScene.addRenderPass("DefaultRenderPass", Renderpasses::createDefaultRenderPass(Epsilon::getContext()));
			m_pScene.addRenderPass("ForwardRenderPass", m_pForwardRenderPass);

			m_pScene.addBindGroup("DefaultBindGroup", 0, {
					.bindingInfo = {},
					.inputs = {},
					.renderPass = "Forward",
					.name = "DefaultBindGroup",
				});


			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 5; j++) {


					auto cubeNode = m_pScene.emplaceIntoScene<engine::Scene::SceneEntity>(engine::Box{ glm::vec3(0.0), glm::vec3(1.0) });

					cubeNode->data.position = glm::vec3((i * 1.5) - 2.5f, 0.0, (j * 1.5) - 2.5f);
					cubeNode->data.scale = glm::vec3(0.5);

					auto script = m_pScene.emplaceIntoNode<EntityScript>(cubeNode);
					script->data.ManagedPtr = host.assembly.Invoke<void*>(L"CreateEntity", cubeNode.get(), L"Game.GameObject");

					if (script->data.ManagedPtr == nullptr) {
						std::cerr << "ManagedPtr is null" << std::endl;
						exit();
					}

					//int id = host.assembly.Invoke<int>(L"InstantiateEntity", eargs);

					const char* str = host.assembly.Invoke<const char*>(L"getEntityFields", script->data.ManagedPtr);
					std::cout << str << std::endl;
					auto props = UI::Property::DeserializeProperties(std::string(str));// m_pObjectProperty.setProperties(std::string(str));
					script->data.properties = props;
					script->data.className = "Game.GameObject";

					engine::Cube cube;
					auto m_pDefaultCube = m_pScene.getAssetManager().createModelFromMesh("DefaultCube", cube.data(), {});

					auto renderNode = m_pScene.insertIntoNode(engine::Box{ glm::vec3(cubeNode->data.position), glm::vec3(0.5) }, cubeNode, m_pDefaultCube);


					UI::NodeProperties node_props;
					node_props.name = "Node_" + std::to_string(cubeNode->Index());
					node_props.mType = UI::SceneNodeType::Node;

					UI::NodeProperties render_props;
					render_props.name = "Render Model" + std::to_string(renderNode->Index());
					render_props.mType = UI::SceneNodeType::Render;
					render_props.node_ref = (void*)&renderNode.get()->data;
					render_props.scene_node_ref = &cubeNode.get()->data;
					render_props.index = cubeNode->Index();

					UI::NodeProperties script_props;
					script_props.name = "Script";
					script_props.mType = UI::SceneNodeType::Script;
					script_props.node_ref = (void*)&script.get()->data;
					script_props.scene_node_ref = &cubeNode.get()->data;
					script_props.index = cubeNode->Index();

					node_props.children.push_back(render_props);
					node_props.children.push_back(script_props);
					node_props.node_ref = (void*)&cubeNode.get()->data;
					node_props.model_path = "DefaultCube";
					node_props.scene_node_ref = &cubeNode.get()->data;
					node_props.index = cubeNode->Index();
					m_pSceneNodes.PushInRoot(node_props);

				}
			auto renderer = getContext()->Renderer();

			renderer->InitDebugRenderer();
			renderer->getDebugRenderer()->ShowDebugPerformance(false);

			renderer->getDebugRenderer()->setUserFunction([this]() {

				m_pMenuBar.draw();
				m_pTooldbar.draw();
				m_pInspector.draw();
				m_pMainViewport.setImage(getContext()->Renderer()->getDebugRenderer()->getImages().at("Forward0"));
				m_pMainViewport.draw();
				m_pSceneNodes.draw();
				selected_entity = (engine::Scene::SceneEntity*)m_pSceneNodes.scene_node_ref;// reinterpret_cast<std::shared_ptr<engine::Scene::SceneEntity>> (m_pSceneNodes.scene_node_ref);
				selected_index = m_pSceneNodes.selected_index;
				glm::mat4 original_transform = m_pInspector.getTransform();
				selected_matrix = m_pInspector.getTransform();
				if (selected_entity != nullptr)
					m_pGuizmo.prepare(&selected_matrix, m_pMainViewport.getMode(), m_pCamera->getViewMatrix(), m_pCamera->getProjectionMatrix());

				if (m_pMainViewport.getMode() != UI::NONE && !m_pNavigation && selected_entity != nullptr) {
					m_pGuizmo.draw();
				}

				if (original_transform != selected_matrix && selected_entity != nullptr) {
					auto models = m_pScene.getNodes <engine::RenderModel>();
					auto transform = std::static_pointer_cast<engine::Node<engine::Scene::SceneEntity>>((*models.front())->Parent());
					glm::vec3 a;
					glm::vec4 b;
					glm::decompose(selected_matrix, selected_entity->scale, selected_entity->rotation, selected_entity->position, a, b);

					m_pInspector.setTransform(selected_matrix);
					Transform t;
					t.position[0] = selected_entity->position.x;
					t.position[1] = selected_entity->position.y;
					t.position[2] = selected_entity->position.z;

					t.scale[0] = selected_entity->scale.x;
					t.scale[1] = selected_entity->scale.y;
					t.scale[2] = selected_entity->scale.z;

					EntityArgs args;
					args.transform = t;
					args.id = 0;
				}

				if (selected_index > 0) {
					auto e = m_pScene.getNode(selected_index);
					auto s = m_pScene.getChild<EntityScript>(e);
					auto script = std::static_pointer_cast<engine::Node<EntityScript>>(s);
					m_pObjectProperty.setProperties(script->data.properties);

					m_pObjectProperty.draw();

					for (auto& property : m_pObjectProperty.getProperties()) {
						std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

						std::wstring name = converter.from_bytes(property.name);
						if (property.type == UI::PropertyType::NUMBER) {
							host.assembly.Invoke<void>(L"SetProperty", script->data.ManagedPtr, property.name.c_str(), std::any_cast<float>(property.value));
						}if (property.type == UI::PropertyType::BOOL) {
							host.assembly.Invoke<void>(L"SetPropertyBool", script->data.ManagedPtr, property.name.c_str(), std::any_cast<bool>(property.value));
						}
					}
					script->data.properties = m_pObjectProperty.getProperties();
				}

				});
			}
		};
	}