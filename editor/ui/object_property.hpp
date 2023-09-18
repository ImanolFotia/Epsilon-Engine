#pragma once

#include "ui_element.hpp"
#include <nlohmann/json.hpp>

namespace Editor::UI {
	class UIObjectProperty : public UIElement {
	public:

		std::vector<Property> m_pProperties;

		const std::vector<Property>& getProperties() {
			return m_pProperties;
		}

		void setProperties(std::vector<Property> props) {
			m_pProperties = props;
		}

		void setProperties(const std::string& props) {

			nlohmann::json data = nlohmann::json::parse(props);

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
				m_pProperties.push_back(prop);
			}
		}

		void draw() override {
			ImGui::Begin("Properties");
			for (auto& prop : m_pProperties) {
				if (prop.type == PropertyType::NUMBER) {
					bool hasRange = false;
					float min = 0, max = 0;
					float x = std::any_cast<float>(prop.value);
					for (auto& attribute : prop.attributes) {
						if (attribute->name == "RangeAttribute") {
							hasRange = true;
							min = std::static_pointer_cast<RangeAttribute>(attribute)->min;
							max = std::static_pointer_cast<RangeAttribute>(attribute)->max;
						}
					}
					if (hasRange) {
						ImGui::SliderFloat(prop.name.c_str(), &x, min, max);
					}
					else {
						ImGui::DragFloat(prop.name.c_str(), &x, 0.01f);
					}
					prop.value = x;
				} else if (prop.type == PropertyType::BOOL) {
					bool x = std::any_cast<bool>(prop.value);
					ImGui::Checkbox(prop.name.c_str(), &x);
					prop.value = x;
				}
				else if(prop.type == PropertyType::STRING) {
					std::string x = std::any_cast<std::string>(prop.value);
					ImGui::InputText(prop.name.c_str(), x.data(), x.size(), ImGuiInputTextFlags_ReadOnly);
					prop.value = x;
				}
				else {}
			}
			ImGui::End();
		}
	};
}