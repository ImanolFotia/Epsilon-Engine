//
// Created by solaire on 2/3/23.
//
#include "Epsilon.hpp"

namespace Epsilon {
    Epsilon::Epsilon(const std::string &appName) {
        self.m_ApplicationName = appName;
        engine::Context::getSingleton().Init(appName, engine::renderer_type::vulkan);
    }
}