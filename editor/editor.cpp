#include "editor.hpp"

namespace Editor {
	void Editor::OnCreate() {


		m_pMenuBar.addCallback("OnExit", [this]() { mShouldClose = true; });

		m_pCamera = std::make_shared<utils::Camera>(glm::vec3(5.0, 5.0, 4.5), glm::vec3(0.0) - glm::vec3(5.0, 5.0, 4.5));

		m_pInspector.setTransform(selected_matrix);
		m_pInspector.setName("Cube");

		//pLoadDotnet();

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

		auto planeNode = m_pScene.emplaceIntoScene<engine::Scene::SceneEntity>(engine::Box{ glm::vec3(0.0f), glm::vec3(1.0) });

		planeNode->data.transform = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -1.0f, 0.0));
		planeNode->data.transform = glm::scale(planeNode->data.transform, glm::vec3(10.0, 1.0, 10.0));
		planeNode->data.transform = glm::rotate(planeNode->data.transform, glm::radians(270.f), glm::vec3(1.0, 0.0, 0.0));

		engine::Quad quad;
		auto m_pDefaultCube = m_pScene.getAssetManager().createModelFromMesh("DefaultPlane", quad.data(), {});
		auto renderNode = m_pScene.insertIntoNode(engine::Box{ glm::vec3(planeNode->data.transform[3]), glm::vec3(10.0) }, planeNode, m_pDefaultCube);

		//////


		auto script = m_pScene.emplaceIntoNode<EntityScript>(planeNode);
		script->data.ManagedPtr = host.assembly.Invoke<void*>(L"CreateEntity", planeNode.get(), L"Game.Ground", ("Node_" + std::to_string(planeNode->Index())).c_str());

		if (script->data.ManagedPtr == nullptr) {
			std::cerr << "ManagedPtr is null" << std::endl;
			exit();
		}

		const char* str = host.assembly.Invoke<const char*>(L"getEntityFields", script->data.ManagedPtr);
		std::cout << str << std::endl;
		auto props = UI::Property::DeserializeProperties(std::string(str));// m_pObjectProperty.setProperties(std::string(str));
		script->data.properties = props;
		script->data.className = "Game.Ground";

		EntityArgs args;
		args.transform = toTransform(planeNode->data.transform);
		args.id = 0;
		host.assembly.Invoke<void>(L"setEntityTransform", script->data.ManagedPtr, args);

		/////

		UI::NodeProperties node_props;
		node_props.name = "Node_" + std::to_string(planeNode->Index());
		node_props.mType = UI::SceneNodeType::Node;

		UI::NodeProperties render_props;
		render_props.name = "Render Model" + std::to_string(renderNode->Index());
		render_props.mType = UI::SceneNodeType::Render;
		render_props.node_ref = (void*)&renderNode.get()->data;
		render_props.scene_node_ref = &planeNode.get()->data;
		render_props.index = planeNode->Index();

		UI::NodeProperties script_props;
		script_props.name = "Script";
		script_props.mType = UI::SceneNodeType::Script;
		script_props.node_ref = (void*)&script.get()->data;
		script_props.scene_node_ref = &planeNode.get()->data;
		script_props.index = planeNode->Index();

		node_props.children.push_back(render_props);
		node_props.node_ref = (void*)&planeNode.get()->data;
		node_props.model_path = "DefaultPlane";
		node_props.scene_node_ref = &planeNode.get()->data;
		node_props.index = planeNode->Index();
		m_pSceneNodes.PushInRoot(node_props);

		for (int i = 0; i < 5; i++)
			for (int j = 0; j < 5; j++) {
				pAddDefaultCube(glm::vec3(i - 2.5f, 10.0, j - 2.5f));

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
				auto e = std::static_pointer_cast<engine::Node<engine::Scene::SceneEntity>>(m_pScene.getNode(selected_index));
				EntityArgs args;
				args.transform = toTransform(selected_matrix);
				e->data.transform = selected_matrix;
				args.id = 0;
			}

			if (selected_index > 0) {
				auto e = m_pScene.getNode(selected_index);
				auto s = m_pScene.getChild<EntityScript>(e);
				auto script = std::static_pointer_cast<engine::Node<EntityScript>>(s);
				m_pObjectProperty.setProperties(s->data.className, script->data.properties);
				m_pObjectProperty.SetNode(m_pSceneNodes.selected_node);
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

	void Editor::OnUpdate() {

		auto models = m_pScene.getNodes <engine::RenderModel>();
		auto scripts = m_pScene.getNodes <EntityScript>();


		for (auto s : scripts) {
			auto script = std::static_pointer_cast<engine::Node<EntityScript>>(*s);
			auto transform = std::static_pointer_cast<engine::Node<engine::Scene::SceneEntity>>(script->Parent());

			if (m_pTooldbar.getStatus() == UI::Toolbar::STATUS::Playing)
				host.assembly.Invoke<void>(L"UpdateEntity", script->data.ManagedPtr, (float)framework::Clock::DeltaSeconds());
			else {

				EntityArgs args;
				args.transform = toTransform(transform->data.transform);
				args.id = 0;
				host.assembly.Invoke<void>(L"setEntityTransform", script->data.ManagedPtr, args);
			}

		}

		if (selected_entity != nullptr) {
			selected_matrix = selected_entity->transform;
			m_pInspector.setTransform(selected_entity->transform);

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

	void Editor::OnRender() {

		m_pScene.BeginScene();

		m_pScene.setCurrentRenderPass("ForwardRenderPass");

		auto models = m_pScene.getNodes <engine::RenderModel>();

		for (auto& model_node : models) {
			auto model = std::static_pointer_cast<engine::Node<engine::RenderModel>>(*model_node);
			auto transform = std::static_pointer_cast<engine::Node<engine::Scene::SceneEntity>>(model->Parent());
			m_pScene.Push(model, transform->data.transform, "DefaultBindGroup");
		}
		m_pScene.Flush();

		m_pScene.EndScene();

	}


	void Editor::pAddDefaultCube(glm::vec3 position) {

		auto cubeNode = m_pScene.emplaceIntoScene<engine::Scene::SceneEntity>(engine::Box{ position, glm::vec3(1.0) });

		cubeNode->data.transform = glm::translate(glm::mat4(1.0), position);
		cubeNode->data.transform = glm::scale(cubeNode->data.transform, glm::vec3(0.4f));

		auto script = m_pScene.emplaceIntoNode<EntityScript>(cubeNode);
		script->data.ManagedPtr = host.assembly.Invoke<void*>(L"CreateEntity", cubeNode.get(), L"Game.GameObject", ("Node_" + std::to_string(cubeNode->Index())).c_str());

		if (script->data.ManagedPtr == nullptr) {
			std::cerr << "ManagedPtr is null" << std::endl;
			exit();
		}

		const char* str = host.assembly.Invoke<const char*>(L"getEntityFields", script->data.ManagedPtr);
		std::cout << str << std::endl;
		auto props = UI::Property::DeserializeProperties(std::string(str));// m_pObjectProperty.setProperties(std::string(str));
		script->data.properties = props;
		script->data.className = "Game.GameObject";

		//auto model = m_pScene.getAssetManager().loadModel("models/pot.gltf");
		engine::Sphere sphere(10);
		auto m_pDefaultCube = m_pScene.getAssetManager().createModelFromMesh("DefaultCube", sphere.data(), {});
		auto renderNode = m_pScene.insertIntoNode(engine::Box{ glm::vec3(cubeNode->data.transform[3]), glm::vec3(0.5) }, cubeNode, m_pDefaultCube);

		EntityArgs args;
		args.transform = toTransform(cubeNode->data.transform);
		args.id = 0;
		host.assembly.Invoke<void>(L"setEntityTransform", script->data.ManagedPtr, args);

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
}