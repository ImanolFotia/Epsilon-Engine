#pragma once

#include <imgui.h>
#include <string>

namespace Editor::UI {

	enum TransformationMode {
		TRANSLATE,
		SCALE,
		ROTATE,
		BOUND,
		ALL,
		NONE
	};
	class UIElement {
	public:
		virtual void draw() = 0;
	};

	struct Attribute {
		std::string getName() {
			return name;
		}
		std::string name;
	};

	struct RangeAttribute : Attribute {
		RangeAttribute() {
			name = "RangeAttribute";
		}
		float min = 0;
		float max = 0;
	};

	enum PropertyType {
		NUMBER,
		STRING,
		BOOL,
		EMPTY
	};
	struct Property {
		std::any value;
		PropertyType type = EMPTY;
		std::string name;
		std::vector<std::shared_ptr<Attribute>> attributes;

		static std::vector<Property> DeserializeProperties(const std::string& str) {

			nlohmann::json data = nlohmann::json::parse(str);
			std::vector<Property> properties;

			for (auto& [key, property] : data.items()) {
				std::cout << "key: " << key << " property: " << property << std::endl;

				Property prop;
				if (property["Value"].is_string()) {
					prop.name = key;
					prop.type = PropertyType::STRING;
					prop.value = std::string(property["Value"].get<std::string>());
				}
				else if (property["Value"].is_boolean()) {
					prop.name = key;
					prop.type = PropertyType::BOOL;
					prop.value = property["Value"].get<bool>();
				}

				else if (property["Value"].is_number()) {
					prop.name = key;
					prop.type = PropertyType::NUMBER;
					prop.value = property["Value"].get<float>();
				}

				for (auto& [name, attribute] : property["Attributes"].items()) {
					if (name == "RangeAttribute") {
						auto attr_ptr = std::make_shared<RangeAttribute>();//prop.attributes.emplace_back<RangeAttribute>();
						attr_ptr->min = attribute["Min"].get<float>();
						attr_ptr->max = attribute["Max"].get<float>();
						prop.attributes.push_back(attr_ptr);
					}
					std::cout << attribute << std::endl;
				}
				properties.push_back(prop);
			}

			return properties;
		}

	};

	enum class SceneNodeType : int {
		Root,
		Node,
		Render,
		Script,
		Transform
	};

	struct NodeProperties {

		bool valid() {
			return node_ref != nullptr;
		}

		size_t num_children() {
			return children.size();
		}

		void DestroyChildren() {
			for (auto& child : children) {
				child.DestroyChildren();
			}

			children.clear();
		}

		const char* type() {
			switch ((int)mType) {
			case (int)SceneNodeType::Render: return "Render";
			case (int)SceneNodeType::Transform: return "Transform";
			case (int)SceneNodeType::Script: return "Script";
			default: return "Node"; break;
			}
		}

		void* node_ref = nullptr;
		void* scene_node_ref = nullptr;
		std::string name{};
		std::string model_path{};

		std::vector<NodeProperties> children;

		SceneNodeType mType{};

		uint32_t index = 0;

		uint32_t Index() {
			return index;
		}

	};
}

namespace Editor {
	struct EntityScript {
		std::string className{};
		void* ManagedPtr;
		std::vector<UI::Property> properties;
	};
};