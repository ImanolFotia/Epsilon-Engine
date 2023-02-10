//
// Created by solaire on 2/3/23.
//

#ifndef EPSILON_MAIN_CPP_HPP
#define EPSILON_MAIN_CPP_HPP

#include "MonoManaged.hpp"

static void CreateWindow(MonoString *name, int w, int h) {
    engine::Context::getSingleton().Window().init(mono_string_to_utf8(name), w, h);
    Epsilon::Epsilon::getSingleton().m_ApplicationName = mono_string_to_utf8(name);
}
int main() {

    MonoManaged::MonoManaged app;
    app.run();
}

#endif //EPSILON_MAIN_CPP_HPP
