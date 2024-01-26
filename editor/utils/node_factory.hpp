#pragma once

#include <memory>

#include <core/engine/scene/scene.hpp>

#include "../types/camera_info.hpp"
#include "../types/script.hpp"
#include <modules/dotnet/src/host.hpp>
#include "../utils/transform.hpp"
#include "../types/camera_info.hpp"
#include <core/framework/utils/helpers/camera.hpp>


namespace Editor::Utils {

	static std::shared_ptr<engine::Node<engine::Scene::SceneEntity>> CreateNode(glm::mat4 transform, std::shared_ptr<engine::Scene> scene) {

		auto node = scene->emplaceIntoScene<engine::Scene::SceneEntity>(engine::Box{ glm::vec3(0.0), glm::vec3(1.0) });
		node->data.transform = glm::translate(glm::mat4(1.0), glm::vec3(0.0));
		node->data.transform = glm::scale(node->data.transform, glm::vec3(1.0f));

		node->data.transform = transform;

		return node;

	}

	static std::shared_ptr<engine::NodeBase> createNode(glm::vec3 position, glm::vec3 scale, glm::quat rotation, std::shared_ptr<engine::Scene> scene) {

		auto node = scene->emplaceIntoScene<engine::Scene::SceneEntity>(engine::Box{ glm::vec3(0.0), glm::vec3(1.0) });
		node->data.transform = glm::translate(glm::mat4(1.0), position);
		node->data.transform *= glm::mat4(rotation);
		node->data.transform = glm::scale(node->data.transform, scale);

		return node;

	}

	static void AddModelNode(const std::string& path, std::shared_ptr<engine::Scene> scene, std::shared_ptr<engine::Node<engine::Scene::SceneEntity>> parent, const std::string bindGroup = "DefaultBindGroup") {

		auto model = scene->getAssetManager()->loadModel(path);
		model.bindGroup = bindGroup;
		auto renderNode = scene->insertIntoNode(engine::Box{ glm::vec3(parent->data.transform[3]), glm::vec3(0.5) }, parent, model);
		renderNode->data.bindGroup = bindGroup;
		renderNode->data.bindGroupId = std::hash<std::string>{}(bindGroup);

	}

	static void AddModelNode(const std::string& name, std::shared_ptr<engine::Scene> scene, engine::RenderModel& renderModel, std::shared_ptr<engine::Node<engine::Scene::SceneEntity>> parent, const std::string bindGroup = "DefaultBindGroup") {

		renderModel.bindGroup = bindGroup;
		renderModel.name = name;
		auto renderNode = scene->insertIntoNode(engine::Box{ glm::vec3(parent->data.transform[3]), glm::vec3(0.5) }, parent, renderModel);
		renderNode->data.bindGroup = bindGroup;
		renderNode->data.bindGroupId = std::hash<std::string>{}(bindGroup);

	}

	static void AddCameraNode(const std::string& name, std::shared_ptr<engine::Scene> scene, std::shared_ptr<engine::Node<engine::Scene::SceneEntity>> parent, CameraCreationInfo cameraCreationInfo) {

		utils::Camera camera(cameraCreationInfo.position, cameraCreationInfo.direction);
		camera.setFoV(cameraCreationInfo.Fov);

		scene->insertIntoNode(parent, camera);

	}



	static void AddScriptNode(ScriptCreationInfo creationInfo, std::shared_ptr<engine::Scene> scene, std::shared_ptr<engine::Node<engine::Scene::SceneEntity>> parent, dotnet::DotnetHost& host) {

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

}