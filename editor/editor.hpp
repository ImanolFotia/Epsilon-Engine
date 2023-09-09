#pragma once

#include <Epsilon.hpp>

#include "renderpasses/default.hpp"
#include "renderpasses/forward.hpp"

#include "ui/menu_bar.hpp"
#include "ui/main_viewport.hpp"

#include <core/engine/scene/scene.hpp>

#include <core/engine/renderer/drawables/primitives/sphere.hpp>
#include <core/framework/utils/helpers/camera.hpp>

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

			getContext()->Renderer()->BeginFrame();
			getContext()->Renderer()->Begin();
			auto v = glm::vec3(0.0);


			getContext()->Renderer()->Push({
				.mesh = m_pDefaultCube,
				.material = m_pForwardBindGroup,
				.objectConstant = {glm::mat4(1.0), 0},
				.layout_index = 0,
				.uniformIndex = 0,
				.count = 1
				});

			getContext()->Renderer()->Flush(m_pForwardRenderPass, engine::DrawType::INDEXED);

			getContext()->Renderer()->End(v);
			getContext()->Renderer()->Submit();
		}

		void OnExit() {
			getContext()->CleanUp();
		}

		void OnUpdate() {
			const auto& [screenX, screenY] = getWindowDimensions();
			m_pCamera->Update(Epsilon::getContext()->Window().getWindow());
			m_pCamera->UpdateMatrices(0, screenX, screenY, false);
			shaderData.iResolution = glm::vec2(screenX, screenY);


			static auto startTime = std::chrono::high_resolution_clock::now();
			auto currentTime = std::chrono::high_resolution_clock::now();
			auto time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

			shaderData.view = m_pCamera->getViewMatrix();
			shaderData.proj = m_pCamera->getProjectionMatrix();
			shaderData.proj[1][1] *= -1;
			shaderData.iTime += time;
			shaderData.viewPosition = m_pCamera->getPosition();

			getContext()->Renderer()->UpdateRenderPassUniforms(m_pForwardRenderPass, engine::RENDERPASS_SET, &shaderData);

		}

		void OnCreate() {

			m_pDefaultRenderPass = Renderpasses::createDefaultRenderPass(Epsilon::getContext());

			m_pForwardRenderPass = Renderpasses::createForwardRenderPass(Epsilon::getContext());


			engine::BindGroupInfo defaultBindGroup = {
					.bindingInfo = {},
					.inputs = {},
					.renderPass = "Forward",
					.name = "DefaultBindGroup",
			};

			m_pForwardBindGroup = getContext()->ResourceManager()->createBindGroup(defaultBindGroup);

			engine::Sphere sphere(10);

			m_pDefaultCube = getContext()->ResourceManager()->createMesh({
				.vertices = sphere.data().Vertices,
				.indices = sphere.data().Indices,
				.name = "DefaultCube"
			});

			Epsilon::getContext()->Renderer()->InitDebugRenderer();

			Epsilon::getContext()->Renderer()->getDebugRenderer()->ShowDebugPerformance(false);

			Epsilon::getContext()->Renderer()->getDebugRenderer()->setUserFunction([this]() {
				m_pMenuBar.draw();
				m_pMainViewport.setImage(getContext()->Renderer()->getDebugRenderer()->getImages().at("Forward0"));
				m_pMainViewport.draw();
			});


		}
	};
}