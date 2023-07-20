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
    AndroidApp() = default;

    ANativeWindow* mWindow;
    bool allSet = false;

    explicit AndroidApp(const std::string &appName, ANativeWindow* window) : Epsilon::Epsilon(appName) {
       /* try {
            mWindow = window;
            m_pContext->Window().setWindow(window);
            m_pContext->Init(appName, engine::renderer_type::vulkan);
          } catch(std::exception& e) {
              std::cout << e.what() << std::endl;
              std::exit(255);
          }*/


        Epsilon::onCreate = [this]
        { onCreate(); };
        Epsilon::onReady = [this]
        { onReady(); };
        Epsilon::onUpdate = [this]
        { onUpdate(); };
        Epsilon::onRender = [this]
        { onRender(); };
        Epsilon::onExit = [this]
        { onExit(); };
    }

    ANativeWindow* getWindow() {
        return  m_pContext->Window().getWindow();
    }

    void setWindow(ANativeWindow* window) {
        return  m_pContext->Window().setWindow(window);
    }

    void Init() {

        m_pContext->Init("Android Epsilon", engine::renderer_type::vulkan);
        allSet = true;
    }

/*
    void run() {
        initWindow();
        initVulkan();
        onCreate();
        onReady();
        mainLoop();
        exit();
    }*/

    virtual void onCreate(){}

    virtual void onReady(){}

    virtual void onRender(){}

    virtual void onExit() {}
};


#endif //EPSILON_ANDROIDAPP_HPP
