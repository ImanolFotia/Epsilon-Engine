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


struct AppData {
    struct android_app* app_context;
    AndroidApp* myApp;
};

/**
 * Called by the Android runtime whenever events happen so the
 * app can react to it.
 */
static void HandleCmd(struct android_app *app, int32_t cmd) {
    auto *appData = (AppData *)app->userData;
    switch (cmd) {
        case APP_CMD_START:
            if (appData->app_context->window != nullptr) {
                appData->myApp->setWindow(appData->app_context->window);
                //appData->myApp->initVulkan();
                //engine->app_backend->Init();
                appData->myApp->canRender = true;
            }
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            LOGI("Called - APP_CMD_INIT_WINDOW");
            if (appData->app_context->window != nullptr) {
                LOGI("Setting a new surface");
                appData->myApp->setWindow(appData->app_context->window);
                if (!appData->myApp->allSet) {
                    LOGI("Starting application");
                    //myApp->Init();
                    appData->myApp->initVulkan();
                    appData->myApp->allSet = true;

                }
                appData->myApp->canRender = true;
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            appData->myApp->canRender = false;
            break;
        case APP_CMD_DESTROY:
            // The window is being hidden or closed, clean it up.
            LOGI("Destroying");
            //engine->app_backend->cleanup();
        default:
            break;
    }
}

static void HandleInputEvents(struct android_app *app) {
    auto inputBuf = android_app_swap_input_buffers(app);
    if (inputBuf == nullptr) {
        return;
    }

    // For the minimum, apps need to process the exit event (for example,
    // listening to AKEYCODE_BACK). This sample has done that in the Kotlin side
    // and not processing other input events, we just reset the event counter
    // inside the android_input_buffer to keep app glue code in a working state.
    android_app_clear_motion_events(inputBuf);
    android_app_clear_motion_events(inputBuf);
}

void android_main(struct android_app *state) {
    AppData appData;
    AndroidApp myApp;
    myApp.Init();

    appData.myApp = &myApp;
    appData.app_context = state;

    state->userData = &appData;
    state->onAppCmd = HandleCmd;

    //android_app_set_key_event_filter(state, VulkanKeyEventFilter);
    //android_app_set_motion_event_filter(state, VulkanMotionEventFilter);

    if (state->window != nullptr) {
        myApp.setWindow(state->window);
    }

    while (true) {
        int ident;
        int events;
        android_poll_source *source;
        while ((ident = ALooper_pollAll(myApp.allSet ? 0 : -1, nullptr, &events,
                                        (void **)&source)) >= 0) {
            if (source != nullptr) {
                source->process(state, source);
            }
        }

        HandleInputEvents(state);

        if(myApp.canRender)
            myApp.onRender();
    }
}