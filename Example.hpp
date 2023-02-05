#pragma once
#define GLM_FORCE_RADIANS

#include "Epsilon.hpp"
#include <core/engine/renderer/drawables/primitives/quad.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <core/engine/renderer/drawables/primitives/cube.hpp>
#include <core/engine/renderer/drawables/primitives/sphere.hpp>

namespace ExampleApp {
    class ExampleApp : public Epsilon::Epsilon {
    public:
        explicit ExampleApp(const std::string &appName) {}

        void onReady() {}
        void onCreate() {}
        void onRender() {}
        void onExit() {}
    };
}