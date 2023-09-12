#pragma once

#include <imgui.h>

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
}