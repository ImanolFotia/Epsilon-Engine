#pragma once

#include <string>

namespace Editor {

	struct NodeType {
		enum NodeTypes : uint16_t {
			EmptyNode = 0,
			RenderModel,
			Sound,
			Script,
			Sprite,
			RigidBody,
			SoftBody,
			Camera,
			Size
		};

		static inline const char* NodeTypesNames[NodeTypes::Size] = {
			ICON_FA_BORDER_NONE "  Empty Node",
			ICON_FA_CUBES "  Render Model",
			ICON_FA_VOLUME_UP "  Sound",
			ICON_FA_CODE "  Script",
			ICON_FA_IMAGE "  Sprite",
			ICON_FA_CUBE "  Rigid Body",
			ICON_FA_MAP "  Soft Body",
			ICON_FA_CAMERA "  Camera"
		};
	};

	struct EditorNode {
		std::string name;
		std::string model_name;
	};
}