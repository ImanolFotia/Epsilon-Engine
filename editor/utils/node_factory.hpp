#pragma once

#include <memory>

#include <core/engine/scene/scene.hpp>

#include "../types/camera_info.hpp"
#include "../types/script.hpp"
#include <modules/dotnet/src/host.hpp>

namespace Editor::Utils {

	static std::shared_ptr<engine::Node<engine::Scene::SceneEntity>> CreateNode(glm::mat4 transform, engine::Scene* scene) {

		auto node = scene->emplaceIntoScene<engine::Scene::SceneEntity>(engine::Box{ glm::vec3(0.0), glm::vec3(1.0) });
		node->data.transform = glm::translate(glm::mat4(1.0), glm::vec3(0.0));
		node->data.transform = glm::scale(node->data.transform, glm::vec3(1.0f));

		node->data.transform = transform;

		return node;

	}

	static std::shared_ptr<engine::NodeBase> createNode(glm::vec3 position, glm::vec3 scale, glm::quat rotation, engine::Scene* scene) {

		auto node = scene->emplaceIntoScene<engine::Scene::SceneEntity>(engine::Box{ glm::vec3(0.0), glm::vec3(1.0) });
		node->data.transform = glm::translate(glm::mat4(1.0), position);
		node->data.transform *= glm::mat4(rotation);
		node->data.transform = glm::scale(node->data.transform, scale);

		return node;

	}

	static void AddModelNode(const std::string& path, engine::Scene* scene, std::shared_ptr<engine::Node<engine::Scene::SceneEntity>> parent, const std::string bindGroup = "DefaultBindGroup") {

		auto& model = scene->getAssetManager().loadModel(path);
		model.bindGroup = bindGroup;
		auto renderNode = scene->insertIntoNode(engine::Box{ glm::vec3(parent->data.transform[3]), glm::vec3(0.5) }, parent, model);
		renderNode->data.bindGroup = bindGroup;

	}

	static void AddModelNode(const std::string& name, engine::Scene* scene, engine::RenderModel& renderModel, std::shared_ptr<engine::Node<engine::Scene::SceneEntity>> parent, const std::string bindGroup = "DefaultBindGroup") {

		renderModel.bindGroup = bindGroup;
		renderModel.name = name;
		auto renderNode = scene->insertIntoNode(engine::Box{ glm::vec3(parent->data.transform[3]), glm::vec3(0.5) }, parent, renderModel);
		renderNode->data.bindGroup = bindGroup;

	}

	static void AddCameraNode(const std::string& name, engine::Scene* scene, std::shared_ptr<engine::Node<engine::Scene::SceneEntity>> parent, CameraCreationInfo cameraCreationInfo) {

		utils::Camera camera(cameraCreationInfo.position, cameraCreationInfo.direction);
		camera.setFoV(cameraCreationInfo.Fov);

		scene->insertIntoNode(parent, camera);

	}



	static void AddScriptNode(ScriptCreationInfo creationInfo, engine::Scene* scene, std::shared_ptr<engine::Node<engine::Scene::SceneEntity>> parent, dotnet::DotnetHost& host) {

		auto script = scene->emplaceIntoNode<EntityScript>(parent);

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wideName = converter.from_bytes(creationInfo.className);

		script->data.ManagedPtr = host.assembly.Invoke<void*>(L"CreateEntity", parent.get(), wideName.c_str(), creationInfo.nodeName.c_str());

		if (script->data.ManagedPtr == nullptr) {
			std::cerr << "ManagedPtr is null" << std::endl;
			throw;
		}

		script->data.updateClbk = host.assembly.getFunction<void, void*, float>(L"UpdateEntity");
		const char* str = host.assembly.Invoke<const char*>(L"getEntityFields", script->data.ManagedPtr);
		//std::cout << str << std::endl;
		auto props = UI::Property::DeserializeProperties(std::string(str));// m_pObjectProperty.setProperties(std::string(str));
		script->data.properties = props;
		script->data.className = creationInfo.className;

		EntityArgs args;
		args.transform = toTransform(parent->data.transform);
		args.id = 0;
		args.objectType = wideName.c_str();
		host.assembly.Invoke<void>(L"setEntityTransform", script->data.ManagedPtr, args);
	}

	static void RegisterIntoEditor(const std::string& name, UI::SceneNodes* scene_nodes, engine::Scene* scene, std::shared_ptr<engine::Node<engine::Scene::SceneEntity>> parent) {

		auto nodeChildren = scene->getChildren<engine::Scene::SceneEntity>(parent);

		UI::NodeProperties node_props;
		node_props.name = name;
		node_props.mType = UI::SceneNodeType::Node;
		node_props.node_ref = (void*)&parent.get()->data;
		node_props.scene_node_ref = &parent.get()->data;
		node_props.index = parent->Index();

		for (auto& [type, children] : nodeChildren) {
			for (auto& child : children) {
				if (scene->isOfType<EntityScript>(child))
				{
					auto script = std::static_pointer_cast<engine::Node<EntityScript>>(child);
					UI::NodeProperties script_props;
					script_props.name = "Script";
					script_props.mType = UI::SceneNodeType::Script;
					script_props.node_ref = (void*)&script.get()->data;
					script_props.scene_node_ref = &parent.get()->data;
					script_props.index = parent->Index();
					node_props.children.push_back(script_props);

				}
				if (scene->isOfType<engine::RenderModel>(child))
				{
					auto renderNode = std::static_pointer_cast<engine::Node<engine::RenderModel>>(child);

					UI::NodeProperties render_props;
					render_props.name = "Render Model" + std::to_string(parent->Index());
					render_props.mType = UI::SceneNodeType::Render;
					render_props.node_ref = (void*)&renderNode.get()->data;
					render_props.scene_node_ref = &parent.get()->data;
					render_props.index = parent->Index();
					node_props.children.push_back(render_props);
				}
			}
		}
		scene_nodes->PushInRoot(node_props);
	}
}