#include "editor.hpp"

#include "utils/mesh_utils.hpp"
#include "ui/style.hpp"

namespace Editor {
	void Editor::OnCreate() {


		m_pMenuBar.addCallback("OnExit", [this]() { mShouldClose = true; });

		m_pCamera = std::make_shared<utils::Camera>(glm::vec3(5.0, 5.0, 4.5), glm::vec3(0.0) - glm::vec3(5.0, 5.0, 4.5));

		m_pInspector.setTransform(selected_matrix);
		m_pInspector.setName("Cube");

		m_pScene = std::make_shared<engine::Scene>(getContext());

		m_pObjectProperty.m_pScenePtr = m_pScene;
		m_pSceneNodes.m_pScenePtr = m_pScene;

		m_pBrushManager = std::make_shared<BrushManager>(getContext());

		m_pAssets = UI::Assets(m_pScene);

		engine::Cube cube;
		m_pBrushManager->addBrush(toTriangleStrip(cube.data().Vertices, cube.data().Indices), 0);

		m_pMaterialEditor.m_pSceneRef = m_pScene;


		host.assembly.Invoke<void>(L"setScenePtr", &m_pScene);

		m_pComputeShader = ComputeDispatch::createComputeShader(m_pScene);

		m_pForwardRenderPass = Renderpasses::createForwardRenderPass(m_pScene);

		m_pScene->addRenderPass("DefaultRenderPass", Renderpasses::createDefaultRenderPass(m_pScene));

		m_pTAAPasses = Renderpasses::createTAARenderPass(m_pScene);

		//m_pScene->getAssetManager().CreateGPUBuffer("TAADataBuffer", sizeof(Renderpasses::TAAUniformData), engine::BufferStorageType::UNIFORM_BUFFER);

		m_pScene->addRenderPass("Forward", m_pForwardRenderPass);

		m_pScene->addRenderPass("TAARenderPass0", m_pTAAPasses.renderpass[0]);
		m_pScene->addRenderPass("TAARenderPass1", m_pTAAPasses.renderpass[1]);

		m_pScene->addBindGroup("DefaultBindGroup", 0, {
				.bindingInfo = {{.size = sizeof(engine::PBRMaterial) * engine::AssetManager::MAX_MATERIALS, .offset = 0, .binding = 1, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "material_buffer"}},
				.inputs = {},
				.renderPass = "Forward",
				.name = "DefaultBindGroup",
			});

		m_pDefaultBindGroup = std::hash<std::string>{}("DefaultBindGroup");

		m_pScene->addBindGroup("SkyBindGroup", 1, {
				.bindingInfo = {},
				.inputs = {},
				.renderPass = "Forward",
				.name = "SkyBindGroup",
			});

		m_pSkyBindGroup = std::hash<std::string>{}("SkyBindGroup");

		m_pScene->addBindGroup("GridBindGroup", 2, {
				.bindingInfo = {},
				.inputs = {},
				.renderPass = "Forward",
				.name = "GridBindGroup",
			});

		m_pGridBindGroup = std::hash<std::string>{}("GridBindGroup");

		m_pScene->addBindGroup("GrassBindGroup", 3, {
				.bindingInfo = {{.size = sizeof(engine::PBRMaterial) * engine::AssetManager::MAX_MATERIALS, .offset = 0, .binding = 1, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "material_buffer"},
								{.size = sizeof(glm::mat4) * engine::AssetManager::MAX_TRANSFORMS, .offset = 0, .binding = 5, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "transform_buffer"},
								{.size = sizeof(engine::ShaderObjectData) * engine::AssetManager::MAX_OBJECTS, .offset = 0, .binding = 6, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "object_buffer"}},
				.inputs = {},
				.renderPass = "Forward",
				.name = "GrassBindGroup",
			});

		m_pGrassBindGroup = std::hash<std::string>{}("GrassBindGroup");

		m_pScene->addBindGroup("TAABindGroup0", 0, {
				.bindingInfo = {
			{.size = sizeof(Renderpasses::TAAUniformData), .offset = 0, .binding = 1, .type = engine::UniformBindingType::UNIFORM_BUFFER, .buffer = "TAADataBuffer", .name = "TAADataBuffer"}},
				.inputs = {
					{.renderPass = "Forward", .index = 0, .bindingPoint = 2},
					{.renderPass = "TAARenderPass1", .index = 0, .bindingPoint = 3},
					{.renderPass = "Forward", .index = 1, .bindingPoint = 4},
					{.renderPass = "Forward", .index = 2, .bindingPoint = 5}
					},
				.renderPass = "TAARenderPass0",
				.name = "TAABindGroup0",
			});

		m_pScene->addBindGroup("TAABindGroup1", 0, {
				.bindingInfo = {
			{.size = sizeof(Renderpasses::TAAUniformData), .offset = 0, .binding = 1, .type = engine::UniformBindingType::UNIFORM_BUFFER, .buffer = "TAADataBuffer", .name = "TAADataBuffer"}},
				.inputs = {
					{.renderPass = "Forward", .index = 0, .bindingPoint = 2},
					{.renderPass = "TAARenderPass0", .index = 0, .bindingPoint = 3},
					{.renderPass = "Forward", .index = 1, .bindingPoint = 4},
					{.renderPass = "Forward", .index = 2, .bindingPoint = 5}
					},
				.renderPass = "TAARenderPass1",
				.name = "TAABindGroup1",
			});


		m_pTAABindGroup0 = std::hash<std::string>{}("TAABindGroup0");
		m_pTAABindGroup1 = std::hash<std::string>{}("TAABindGroup1");

		auto renderer = getContext()->Renderer();

		renderer->InitDebugRenderer();
		setup_style();

		auto node = Utils::CreateNode(glm::mat4(1.0f), m_pScene);
		Utils::AddCameraNode("Camera", m_pScene, node, {});

		m_pSceneNodes.RegisterIntoEditor("Camera", node);

		auto skyNode = m_pScene->emplaceIntoScene<engine::Scene::SceneEntity>(engine::Box{ glm::vec3(0.0f), glm::vec3(1.0) });
		skyNode->data.transform = glm::scale(glm::mat4(1.0), glm::vec3(3.0f));
		engine::Sphere sphere(5);
		auto SkyDomeNode = m_pScene->getAssetManager().createModelFromMesh("Skybox", sphere.data(), {});
		m_pSkybox = m_pScene->insertIntoNode(skyNode, SkyDomeNode);
		m_pSkybox->data.bindGroupId = m_pSkyBindGroup;

		////////////////

		//setMaxFPS(72);

		auto planeNode = m_pScene->emplaceIntoScene<engine::Scene::SceneEntity>(engine::Box{ glm::vec3(0.0f), glm::vec3(1.0) });

		planeNode->data.transform = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0));
		planeNode->data.transform = glm::scale(planeNode->data.transform, glm::vec3(30.0, 1.0, 30.0));
		planeNode->data.transform = glm::rotate(planeNode->data.transform, glm::radians(270.f), glm::vec3(1.0, 0.0, 0.0));

		common::MeshMaterial defaultMaterial;
		defaultMaterial.roughness = 1.0;
		defaultMaterial.metallic = 0;
		defaultMaterial.color = glm::vec4(0.5, 0.5, 0.5, 1.0);
		engine::Quad quad(2, glm::vec2(5.0f), glm::vec3(2.0f));
		auto m_pDefaultCube = m_pScene->getAssetManager().createModelFromMesh("DefaultPlane", quad.data(), defaultMaterial);
		m_pGridPlane = m_pScene->insertIntoNode(engine::Box{ glm::vec3(planeNode->data.transform[3]), glm::vec3(10.0) }, planeNode, m_pDefaultCube);
		m_pGridPlane->data.bindGroupId = m_pDefaultBindGroup;

		auto script = m_pScene->emplaceIntoNode<EntityScript>(planeNode);
		script->data.ManagedPtr = host.assembly.Invoke<void*>(L"CreateEntity", planeNode.get(), L"Game.Ground", "Ground");

		script->data.updateClbk = host.assembly.getFunction<void, void*, float>(L"UpdateEntity");

		if (script->data.ManagedPtr == nullptr) {
			std::cerr << "ManagedPtr is null" << std::endl;
			exit();
		}

		const char* str = host.assembly.Invoke<const char*>(L"getEntityFields", script->data.ManagedPtr);
		//std::cout << str << std::endl;
		auto props = UI::Property::DeserializeProperties(std::string(str));// m_pObjectProperty.setProperties(std::string(str));
		script->data.properties = props;
		script->data.className = "Game.Ground";

		EntityArgs args;
		args.transform = toTransform(planeNode->data.transform);
		args.id = 0;
		host.assembly.Invoke<void>(L"setEntityTransform", script->data.ManagedPtr, args);

		/////

		UI::NodeProperties node_props;
		node_props.name = "Ground";
		node_props.mType = UI::SceneNodeType::Node;

		UI::NodeProperties render_props;
		render_props.name = "Render Model" + std::to_string(m_pGridPlane->Index());
		render_props.mType = UI::SceneNodeType::Render;
		render_props.node_ref = (void*)&m_pGridPlane.get()->data;
		render_props.scene_node_ref = planeNode;
		render_props.index = planeNode->Index();

		UI::NodeProperties script_props;
		script_props.name = "Script";
		script_props.mType = UI::SceneNodeType::Script;
		script_props.node_ref = (void*)&script.get()->data;
		script_props.scene_node_ref = planeNode;
		script_props.index = planeNode->Index();

		node_props.children.push_back(render_props);
		node_props.node_ref = (void*)&planeNode.get()->data;
		node_props.model_path = "DefaultPlane";
		node_props.scene_node_ref = planeNode;
		node_props.index = planeNode->Index();
		m_pSceneNodes.PushInRoot(node_props);

		//////////////////



		m_pScene->getAssetManager().addTexture("textures/radiance.dds", {
			.format = engine::TextureFormat::COLOR_RGBA,
			.wrapMode = engine::CLAMP_TO_EDGE,
			.filtering = engine::LINEAR
			});
		m_pScene->getAssetManager().addTexture("textures/irradiance.dds", {
			.format = engine::TextureFormat::COLOR_RGBA,
			.wrapMode = engine::CLAMP_TO_EDGE,
			.filtering = engine::LINEAR
			});
		std::ifstream file("./assets/lut.bin", std::ios::binary);
		size_t lut_size = 512 * 512 * sizeof(glm::vec2) * 2;
		unsigned char* lut = new unsigned char[lut_size];
		file.read((char*)lut, lut_size);
		m_pScene->getAssetManager().addTextureFromBytes("BRDF_lut", lut, lut_size, 512, 512, 2, {
			.format = engine::TextureFormat::COLOR_RG_32F,
			.wrapMode = engine::CLAMP_TO_EDGE,
			.filtering = engine::LINEAR
			});
		delete[] lut;


		//pAddDefaultCube(glm::vec3(0.0));

		//for (int i = 0; i < 200; i++)
		//	for (int j = 0; j < 200; j++) {
		pAddDefaultPlane(glm::vec3(0.0f));
		//}

		renderer->getDebugRenderer()->ShowDebugPerformance(false);

		renderer->getDebugRenderer()->setUserFunction([this]() {

			m_pMenuBar.draw();
			m_pTooldbar.draw();
			m_pInspector.draw();
			if (m_pPostProcess.TaaEnabled() && !just_resized) {
				m_pMainViewport.setImage(getContext()->Renderer()->getDebugRenderer()->getImages().at(m_pCurrentTAAPass ? "TAATarget0" : "TAATarget1"));
			}
			else {
				m_pMainViewport.setImage(getContext()->Renderer()->getDebugRenderer()->getImages().at("Forward0"));
				just_resized = false;

			}
			m_pMainViewport.draw();
			m_pSceneNodes.draw();
			m_pAssets.draw();
			m_pPostProcess.draw();
			engine::Node<engine::Scene::SceneEntity>* sn;
			if (m_pSceneNodes.scene_node_ref != nullptr)
			{
				sn = static_cast<engine::Node<engine::Scene::SceneEntity>*>(m_pSceneNodes.scene_node_ref);
				selected_entity = &sn->data;// reinterpret_cast<std::shared_ptr<engine::Scene::SceneEntity>> (m_pSceneNodes.scene_node_ref);
			}
			selected_index = m_pSceneNodes.selected_index;
			m_pMaterialEditor.selected_entity = selected_index;
			m_pMaterialEditor.draw();

			glm::mat4 original_transform = m_pInspector.getTransform();
			selected_matrix = m_pInspector.getTransform();
			if (selected_entity != nullptr)
				m_pGuizmo.prepare(&selected_matrix, m_pMainViewport.getMode(), m_pCamera->getViewMatrix(), m_pCamera->getProjectionMatrix());

			if (m_pMainViewport.getMode() != UI::NONE && !m_pNavigation && selected_entity != nullptr) {
				m_pGuizmo.draw();
			}

			if (original_transform != selected_matrix && selected_entity != nullptr) {
				auto e = std::static_pointer_cast<engine::Node<engine::Scene::SceneEntity>>(m_pScene->getNode(selected_index));
				EntityArgs args;
				args.transform = toTransform(selected_matrix);
				e->data.transform = selected_matrix;
				args.id = 0;
			}

			if (selected_index > 0) {
				auto e = m_pScene->getNode(selected_index);
				auto children = m_pScene->getChildren(e);
				m_pObjectProperty.SetNode(m_pSceneNodes.selected_node);

				bool containsScript = false;
				for (auto& child : children) {

					if (child.first == typeid(EntityScript)) {
						containsScript = true;
						break;
					}
				}

				if (containsScript) {

					auto s = m_pScene->getChild<EntityScript>(e);
					auto script = std::static_pointer_cast<engine::Node<EntityScript>>(s);
					m_pObjectProperty.setProperties(s->data.className, script->data.properties);
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
				else {

					m_pObjectProperty.draw();
				}
			}

			ImGui::Begin("Reload Assembly");

			if (ImGui::Button(ICON_FA_REDO, ImVec2(37, 37)))
			{
				host.assembly.Invoke<void>(L"ReloadAssemblies");

				auto scripts = m_pScene->getNodes<EntityScript>();
				for (auto s : scripts) {
					auto script = std::static_pointer_cast<engine::Node<EntityScript>>(s);
					const char* str = host.assembly.Invoke<const char*>(L"getEntityFields", script->data.ManagedPtr);
					auto props = UI::Property::DeserializeProperties(std::string(str));
					script->data.properties = props;
				}
			}


			ImGui::SameLine();
			ImGui::End();

			});
	}

	void Editor::OnUpdate() {

		auto models = m_pScene->getNodes <engine::RenderModel>();
		auto scripts = m_pScene->getNodes <EntityScript>();


		for (auto s : scripts) {
			auto script_node = std::static_pointer_cast<engine::Node<EntityScript>>(s);
			auto& script = script_node->data;
			auto transform = std::static_pointer_cast<engine::Node<engine::Scene::SceneEntity>>(script_node->Parent());

			if (m_pTooldbar.getStatus() == UI::Toolbar::STATUS::Playing)
			{
				EntityArgs args;
				args.transform = toTransform(transform->data.transform);
				args.id = 0;
				host.assembly.Invoke<void>(L"setEntityTransform", script.ManagedPtr, args);

				script.Update((float)framework::Clock::DeltaSeconds());
			}
			else {

				EntityArgs args;
				args.transform = toTransform(transform->data.transform);
				args.id = 0;
				host.assembly.Invoke<void>(L"setEntityTransform", script.ManagedPtr, args);
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


		m_pCamera->UpdateMatrices(Frame(), screenX, screenY, m_pPostProcess.TaaEnabled());

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

		shaderData.prev_view = m_pCamera->getPrevViewMatrix();
		shaderData.prev_proj = shaderData.proj;
		shaderData.view = m_pCamera->getViewMatrix();
		shaderData.proj = m_pCamera->getProjectionMatrix();
		shaderData.proj[1][1] *= -1;
		shaderData.iTime = framework::Clock::TimeSeconds();
		shaderData.viewPosition = m_pCamera->getPosition();
		shaderData.iFrame = Frame();

		getContext()->Renderer()->UpdateRenderPassUniforms(m_pForwardRenderPass, engine::RENDERPASS_SET, &shaderData);

		if (m_pPostProcess.TaaEnabled()) {
			getContext()->Renderer()->UpdateRenderPassUniforms(m_pTAAPasses.renderpass[0], engine::RENDERPASS_SET, &shaderData);
			getContext()->Renderer()->UpdateRenderPassUniforms(m_pTAAPasses.renderpass[1], engine::RENDERPASS_SET, &shaderData);
			getContext()->Renderer()->UpdateRenderPassUniforms(m_pTAAPasses.renderpass[0], (engine::BindingIndex)1, &m_pPostProcess.m_pTAAData);
			getContext()->Renderer()->UpdateRenderPassUniforms(m_pTAAPasses.renderpass[1], (engine::BindingIndex)1, &m_pPostProcess.m_pTAAData);
		}


		if (m_pMainViewport.ShouldResize()) {

			getContext()->ResourceManager()->ResizeFramebuffer(m_pForwardRenderPass, m_pMainViewport.getSize());
			if (m_pPostProcess.TaaEnabled()) {
				getContext()->ResourceManager()->ResizeFramebuffer(m_pTAAPasses.renderpass[0], m_pMainViewport.getSize());
				getContext()->ResourceManager()->ResizeFramebuffer(m_pTAAPasses.renderpass[1], m_pMainViewport.getSize());
				getContext()->ResourceManager()->updateBindGroup(m_pScene->getBindGroups()[m_pTAABindGroup1].bindGroup);
				getContext()->ResourceManager()->updateBindGroup(m_pScene->getBindGroups()[m_pTAABindGroup0].bindGroup);
				just_resized = true;
			}
			Epsilon::getContext()->Renderer()->getDebugRenderer()->recreateDescriptorSets();
			m_pMainViewport.ResetFlags();
		}

	}

	void Editor::OnRender() {

		m_pScene->BeginScene();

		//m_pScene->getContext()->Renderer()->ComputeDispatch(m_pComputeShader);

		m_pScene->setCurrentRenderPass("Forward");

		auto models = m_pScene->getNodes <engine::RenderModel>();

		for (auto& model_node : models) {
			auto model = std::static_pointer_cast<engine::Node<engine::RenderModel>>(model_node);
			auto transform = std::static_pointer_cast<engine::Node<engine::Scene::SceneEntity>>(model->Parent());
			if (model->data.isInstanced) {
				m_pScene->Push(model, model->data.transforms, model->data.bindGroupId, model->data.transforms.size());
			}
			else {
				m_pScene->Push(model, transform->data.transform, model->data.bindGroupId);
			}
		}

		m_pScene->Flush();

		if (m_pPostProcess.TaaEnabled()) {
			m_pScene->setCurrentRenderPass(/**/m_pCurrentTAAPass ? "TAARenderPass0" : "TAARenderPass1");

			m_pScene->Push(m_pGridPlane, glm::mat4(1.0), m_pCurrentTAAPass ? m_pTAABindGroup0 : m_pTAABindGroup1);

			m_pCurrentTAAPass = !m_pCurrentTAAPass;
			m_pScene->Flush();
		}


		m_pScene->EndScene();

	}


	void Editor::pAddDefaultPlane(glm::vec3 position) {

		common::MeshMaterial defaultMaterial;
		defaultMaterial.roughness = 0.34;
		defaultMaterial.metallic = 0;
		defaultMaterial.specular = 0.5;
		defaultMaterial.color = glm::vec4(0.15, 0.6, 0.15, 1.0);
		defaultMaterial.normal_path = "textures/grass_normal.png";
		defaultMaterial.name = "grass_material";

		engine::Quad grass_lod[3] = {
			engine::Quad(8, glm::vec2(1.0 / 8.0, 1.0)),
			engine::Quad(5, glm::vec2(0.2f, 1.0)),
			engine::Quad(3, glm::vec2(0.3f, 1.0))
		};

		for (int lod = 0; lod < 3; lod++) {
			for (auto& vtx : grass_lod[lod].data().Vertices) {
				float tmp = vtx.position.y;
				vtx.position.y = -vtx.position.z;
				vtx.position.z = tmp;

				tmp = vtx.normal.y;
				vtx.normal.y = vtx.normal.z;
				vtx.normal.z = tmp;

				vtx.position.x = vtx.position.x * 2.0f;

				vtx.position.y += 0.5;
			}

			for (auto& vtx : grass_lod[lod].data().Vertices) {

				vtx.position.x *= 0.05f;
				vtx.position.x += 0.02f;
				float y = vtx.position.y;
				vtx.position.x *= 1.0 - (y * y * y * y);
				vtx.position.x *= 0.5f;
			}

			grass_lod[lod].generateTangentSpaceVectors();
		}

		auto GrassBlade_Lod0 = m_pScene->getAssetManager().createModelFromMesh("Grass Blade Lod0", grass_lod[0].data(), defaultMaterial);
		auto GrassBlade_Lod1 = m_pScene->getAssetManager().createModelFromMesh("Grass Blade Lod1", grass_lod[1].data(), defaultMaterial);
		auto GrassBlade_Lod2 = m_pScene->getAssetManager().createModelFromMesh("Grass Blade Lod2", grass_lod[2].data(), defaultMaterial);

		GrassBlade_Lod0.renderMeshes[1] = GrassBlade_Lod1.renderMeshes[0];
		GrassBlade_Lod0.renderMeshes[2] = GrassBlade_Lod2.renderMeshes[0];

		GrassBlade_Lod0.isInstanced = true;

		
		for (int i = 0; i < 200; i++) {
			for (int j = 0; j < 200; j++) {
				//pAddDefaultPlane(glm::vec3((float)(i * 0.15) - 15, 0.0f, (float)(j * 0.15) - 15));
				glm::vec3 position = glm::vec3((float)(i * 0.15) - 15, 0.0f, (float)(j * 0.15) - 15);
				std::random_device rd;
				std::mt19937 e2(rd());

				std::uniform_real_distribution<float> dist(-0.2, 0.2);
				std::uniform_real_distribution<float> dist2(-glm::pi<float>(), glm::pi<float>());

				glm::mat4 transform_matrix = glm::translate(glm::mat4(1.0), position + glm::vec3(dist(e2), 0.0, dist(e2)) + glm::vec3(0.5, 0.0, 0.5));
				glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0), dist2(e2), glm::vec3(0.0, 1.0, 0.0));
				glm::mat4 model_matrix = transform_matrix * rotation_matrix;
				GrassBlade_Lod0.transforms.push_back(model_matrix);

			}
		}

		auto node = Utils::CreateNode(glm::mat4(1.0), m_pScene);


		Utils::AddModelNode("Grass Blades" + std::to_string(node->Index()), m_pScene, GrassBlade_Lod0, node, "GrassBindGroup");

		/*Utils::AddScriptNode({
			.language = C_SHARP,
			.fileName = "GameObject.cs",
			.assemblyName = "Game.dll",
			.className = "Game.GameObject",
			.nodeName = "Plane." + node->Index()
			}, m_pScene, node, host);*/

		m_pSceneNodes.RegisterIntoEditor("Grass", node);
	}

	void Editor::pAddDefaultCube(glm::vec3 position) {

		common::MeshMaterial defaultMaterial;
		defaultMaterial.roughness = 0.8;
		defaultMaterial.metallic = 0;
		defaultMaterial.color = glm::vec4(0.5, 0.5, 0.5, 1.0);



		engine::Cube cube;
		auto m_pDefaultCube = m_pScene->getAssetManager().createModelFromMesh("DefaultCube", cube.data(), defaultMaterial);

		auto node = Utils::CreateNode(glm::mat4(1.0f), m_pScene);

		Utils::AddModelNode("Cube", m_pScene, m_pDefaultCube, node);

		Utils::AddScriptNode({
			.language = C_SHARP,
			.fileName = "GameObject.cs",
			.assemblyName = "Game.dll",
			.className = "Game.GameObject",
			.nodeName = "Cube"
			}, m_pScene, node, host);

		m_pSceneNodes.RegisterIntoEditor("Cube", node);
	}
}