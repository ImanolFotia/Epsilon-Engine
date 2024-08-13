#include <jni.h>
#include <string>

#include "AndroidApp.hpp"
#include <game-activity/native_app_glue/android_native_app_glue.h>

#include <android/log.h>

#define LOG_TAG "hellovkjni"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define VK_CHECK(x)                           \
  do {                                        \
    VkResult err = x;                         \
    if (err) {                                \
      LOGE("Detected Vulkan error: %d", err); \
      abort();                                \
    }                                         \
  } while (0)

extern "C" JNIEXPORT jstring JNICALL
Java_com_imanolfotia_epsilon_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


struct VulkanEngine {
    struct android_app *app;
    AndroidApp *app_backend;
    bool canRender = false;
};


/**
 * Called by the Android runtime whenever events happen so the
 * app can react to it.
 */
static void HandleCmd(struct android_app *app, int32_t cmd) {
    auto *engine = (VulkanEngine *)app->userData;
    switch (cmd) {
        case APP_CMD_START:
            if (engine->app->window != nullptr) {
                engine->app_backend->setWindow(app->window);
                engine->app_backend->Init();
                engine->canRender = true;
            }
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            LOGI("Called - APP_CMD_INIT_WINDOW");
            if (engine->app->window != nullptr) {
                LOGI("Setting a new surface");
                engine->app_backend->setWindow(app->window);
                if (!engine->app_backend->allSet) {
                    LOGI("Starting application");
                    engine->app_backend->Init();
                    engine->app_backend->allSet = true;
                }
                engine->canRender = true;
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            engine->canRender = false;
            break;
        case APP_CMD_DESTROY:
            // The window is being hidden or closed, clean it up.
            LOGI("Destroying");
            //engine->app_backend->cleanup();
        default:
            break;
    }
}

void android_main(struct android_app *state) {
    AndroidApp myApp;
    state->userData = &myApp;
    state->onAppCmd = HandleCmd;

    //android_app_set_key_event_filter(state, VulkanKeyEventFilter);
    //android_app_set_motion_event_filter(state, VulkanMotionEventFilter);

    myApp.run();
/*
    while (true) {
        int ident;
        int events;
        android_poll_source *source;
        while ((ident = ALooper_pollAll(engine.canRender ? 0 : -1, nullptr, &events,
                                        (void **)&source)) >= 0) {
            if (source != nullptr) {
                source->process(state, source);
            }
        }

        //HandleInputEvents(state);

        engine.app_backend->render();
    }*/
}