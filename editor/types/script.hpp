#pragma once

#include <string>
#include <vector>

#include "../ui/ui_element.hpp"

namespace Editor {

	enum SCRIPT_LANGUAGE {
		C_SHARP,
		LUA
	};

	struct ScriptCreationInfo {
		SCRIPT_LANGUAGE language{};
		std::string fileName{};
		std::string assemblyName{};
		std::string className{};
		std::string nodeName{};
	};

	struct EntityScript {
		std::string className{};
		void* ManagedPtr;
		std::vector<UI::Property> properties;
	};

	struct EntityArgs
	{
		const char_t* objectType = L"Game.GameObject";
		Transform transform;
		int id;
	};
}