//
// Created by solaire on 2/2/23.
//

#ifndef EPSILON_ANDROIDAPP_HPP
#define EPSILON_ANDROIDAPP_HPP

#include <jni.h>
#include <android/asset_manager.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include "../../../../../../Epsilon.hpp"

class AndroidApp : public Epsilon::Epsilon {
public:

    bool allSet = false;
    bool canRender = false;

    AndroidApp() = default;

    ANativeWindow* getWindow() {
        return  m_pContext->Window().getWindow();
    }

    void setWindow(ANativeWindow* window) {
        return  m_pContext->Window().setWindow(window);
    }

    void Init() {

        m_pContext = std::make_shared<engine::Context>();
        m_pContext->Init("Android Epsilon", engine::renderer_type::vulkan);
        allSet = false;
    }


    virtual void onCreate(){}

    virtual void onReady(){}

    virtual void onRender(){

        auto renderer = m_pContext->Renderer();
        renderer->BeginFrame();
        renderer->Begin();
        glm::vec3 v;
        renderer->End(v);

        renderer->Submit();
        renderer->EndFrame();
    }

    virtual void onExit() {}
};


#endif //EPSILON_ANDROIDAPP_HPP
