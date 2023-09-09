#pragma once

#include <imgui.h>

namespace Editor::UI {
	class UIElement {
	public:
		virtual void draw() = 0;
	};
}