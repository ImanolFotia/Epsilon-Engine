#pragma once

#include <string>
#include <vector>

#include "../ui/ui_element.hpp"
#include "transform.hpp"
#include <modules/dotnet/headers/nethost.hpp>

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

		std::function<void(void*, float)> updateClbk;

		void Update(float delta) {
			updateClbk(ManagedPtr, delta);
		}
	};

	struct EntityArgs
	{
		const char_t* objectType = L"Game.GameObject";
		Transform transform;
		int id;
	};
}