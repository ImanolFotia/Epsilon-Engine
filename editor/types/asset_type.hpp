#pragma once

#include <string>

namespace Editor {

	enum class AssetType : int {
		Model = 0,
		Audio,
		Texture,
		Script,
		None
	};

	struct AssetProperties {

		const char* type() {
			switch ((int)mType) {
			case (int)AssetType::Model: return "Model";
			case (int)AssetType::Audio: return "Audio";
			case (int)AssetType::Texture: return "Texture";
			case (int)AssetType::Script: return "Script";
			default: return "Unknown Asset"; break;
			}
		}

		std::string name{};

		AssetType mType = AssetType::None;

		uint32_t index = 0;

		uint32_t Index() {
			return index;
		}

	};
}