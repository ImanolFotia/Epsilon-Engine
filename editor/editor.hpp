#pragma once

#include <Epsilon.hpp>

#include "renderpasses/default.hpp"
#include "renderpasses/forward.hpp"

#include "ui/menu_bar.hpp"
#include "ui/main_viewport.hpp"

#include <core/engine/scene/scene.hpp>

#include <core/engine/renderer/drawables/primitives/sphere.hpp>
#include <core/engine/renderer/drawables/primitives/cube.hpp>
#include <core/framework/utils/helpers/camera.hpp>
#include <core/framework/IO/IO.hpp>

namespace Editor {
	class Editor : public Epsilon::Epsilon {

		engine::Ref<engine::RenderPass> m_pDefaultRenderPass;
		engine::Ref<engine::RenderPass> m_pForwardRenderPass;


		engine::Ref<engine::BindGroup> m_pForwardBindGroup;
		engine::Ref<engine::Mesh> m_pDefaultCube;

		UI::MenuBar m_pMenuBar;
		UI::MainViewport m_pMainViewport;

		std::shared_ptr<utils::Camera> m_pCamera;
		ShaderData shaderData;

		engine::Scene m_pScene;

		bool m_pCameraWasInactive = true;



	public:
		Editor(const std::string name) : Epsilon::Epsilon(name) {
			this->setOnCreate([this] { OnCreate(); });
			this->setOnRender([this] { OnRender(); });
			this->setOnExit([this] { OnExit(); });
			this->setOnUpdate([this] {OnUpdate(); });

			m_pMenuBar.addCallback("OnExit", [this]() { mShouldClose = true; });

			m_pCamera = std::make_shared<utils::Camera>(glm::vec3(5.0), glm::vec3(0.0));

		}

		void OnRender() {

			m_pScene.BeginScene();

			m_pScene.setCurrentRenderPass("ForwardRenderPass");
			auto models = m_pScene.getNodes <engine::RenderModel>();

			for (auto& model : models) {
				m_pScene.Push(std::static_pointer_cast<engine::Node<engine::RenderModel>>(*model), glm::mat4(1.0f), "DefaultBindGroup");
			}

			m_pScene.Flush();

			m_pScene.EndScene();

		}

		void OnExit() {
			getContext()->CleanUp();
		}

		void OnUpdate() {
			int screenX, screenY; //getWindowDimensions();
			screenX = m_pMainViewport.getSize().x;
			screenY = m_pMainViewport.getSize().y;
			shaderData.iResolution = glm::vec2(screenX, screenY);

			if (framework::Input::Mouse::LEFT == framework::Input::PRESSED && m_pMainViewport.isHovered()) {

				if (m_pCameraWasInactive) {
					m_pCamera->ResetDeltas();
					m_pCameraWasInactive = false;
				}
				getContext()->Window().HideCursor();
				m_pCamera->Update(Epsilon::getContext()->Window().getWindow());


			}
			else {
				getContext()->Window().ShowCursor();
				m_pCameraWasInactive = true;
			}


			m_pCamera->UpdateMatrices(0, screenX, screenY, false);

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

			m_pForwardRenderPass = Renderpasses::createForwardRenderPass(Epsilon::getContext());

			m_pScene.addRenderPass("DefaultRenderPass", Renderpasses::createDefaultRenderPass(Epsilon::getContext()));
			m_pScene.addRenderPass("ForwardRenderPass", m_pForwardRenderPass);

			m_pScene.addBindGroup("DefaultBindGroup", 0, {
					.bindingInfo = {},
					.inputs = {},
					.renderPass = "Forward",
					.name = "DefaultBindGroup",
			});

			auto cubeNode = m_pScene.emplaceIntoScene<engine::Scene::SceneEntity>(engine::Box{ glm::vec3(0.0), glm::vec3(1.0) });

			engine::Cube cube;
			auto m_pDefaultCube = m_pScene.getAssetManager().createModelFromMesh("DefaultCube", cube.data(), {});

			m_pScene.insertIntoNode(engine::Box{ glm::vec3(0.0), glm::vec3(1.0) }, cubeNode, m_pDefaultCube);

			auto renderer = getContext()->Renderer();

			renderer->InitDebugRenderer();
			renderer->getDebugRenderer()->ShowDebugPerformance(false);

			renderer->getDebugRenderer()->setUserFunction([this]() {
				m_pMenuBar.draw();
				m_pMainViewport.setImage(getContext()->Renderer()->getDebugRenderer()->getImages().at("Forward0"));
				m_pMainViewport.draw();
				});
		}
	};
}