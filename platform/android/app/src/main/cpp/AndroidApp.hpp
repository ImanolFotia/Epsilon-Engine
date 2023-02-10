//
// Created by solaire on 2/2/23.
//

#ifndef EPSILON_ANDROIDAPP_HPP
#define EPSILON_ANDROIDAPP_HPP
/*
#include <jni.h>
#include <android/native_window.h>

#include "../../../../../../Epsilon.hpp"

class AndroidApp : public Epsilon::Epsilon {
public:
    AndroidApp() = default;

    explicit AndroidApp(const std::string &appName, ANativeWindow* window) : Epsilon::Epsilon(appName) {
        //try {
        Epsilon::getContext().Window().setWindow(window);
        Epsilon::getContext().Init(appName, engine::renderer_type::vulkan);
        /*  } catch(std::exception& e) {
              std::cout << e.what() << std::endl;
              std::exit(255);
          }
    }

    void createSurface() {
        VkAndroidSurfaceCreateInfoKHR surface_info;
        surface_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        surface_info.pNext = NULL;
        surface_info.flags = 0;
        surface_info.window = mWindow;
        if(vkCreateAndroidSurfaceKHR(instance, &surface_info, NULL, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void run() {
        initWindow();
        initVulkan();
        onCreate();
        onReady();
        mainLoop();
        exit();
    }

    virtual void onCreate() = 0;

    virtual void onReady() = 0;

    virtual void onRender() = 0;

    virtual void onExit() = 0;
};
*/

#endif //EPSILON_ANDROIDAPP_HPP
