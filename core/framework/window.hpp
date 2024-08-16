#pragma once

#include "core/framework/utils/image.hpp"
#include <string>

#if defined(ANDROID) || defined(__ANDROID__)
#undef USE_GLFW
#include <android/native_window.h>
#include <android/native_window_jni.h>
#else

#include <vulkan/vulkan.hpp>
#endif

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#if USE_GLFW
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#endif

#ifdef __linux__
#define VK_USE_PLATFORM_XLIB_KHR
#endif

#if defined(ANDROID) || defined(__ANDROID__)
#undef VK_USE_PLATFORM_XLIB_KHR
#undef USE_GLFW
#include <android/native_window.h>
#include <jni.h>
#endif

#if USE_GLFW
#include <GLFW/glfw3.h>
#endif

#if _WIN32
//  Microsoft
#if BUILD_SHARED_LIBS
#if defined(EPSILON_BUILD_DLL) && !defined(EPSILON_DLL)
#define EPSILON_DLL __declspec(dllexport)
#endif
#elif defined(__linux__)
//  GCC
#if defined(EPSILON_BUILD_DLL)
#define EPSILON_DLL __attribute__((visibility("default")))
#else
#if !defined(EPSILON_DLL)
#define EPSILON_DLL
#endif
#endif
#else
#if !defined(EPSILON_DLL)
#define EPSILON_DLL
#pragma warning Unknown dynamic link import / export semantics.
#endif
#endif
#endif

#include "common.hpp"
#include <core/framework/log.hpp>

namespace framework {
struct WindowSizeDescription {
  int id{};
  int width{};
  int height{};
  float refreshRate{};
  bool isCurrent = false;
  std::string size_string{};
};

struct WindowSize {
  uint16_t width;
  uint16_t height;
};

class Window {

public:
  enum class WindowMode { WINDOWED = 0, FULLSCREEN, BORDERLESS };
#if USE_GLFW
  using windowType = GLFWwindow;
#elif _WIN32
  using windowType = HWND;
#elif defined(__ANDROID__)
  using windowType = ANativeWindow;
#endif
public:
  void init(std::string appName, int w = 1280, int h = 720);

  void mainLoop() {
#if USE_GLFW
    while (!glfwWindowShouldClose(mWindow)) {
      glfwPollEvents();
    }
#else
#ifdef _WIN32

#endif
#endif
  }

  bool ShouldClose() {
#if USE_GLFW
    if (glfwWindowShouldClose(mWindow)) {
      Log::Info("The OS requested to close the window");
      return true;
    }
#endif
    return false;
  }

  void PollEvents() {
#if USE_GLFW
    glfwPollEvents();
#else
#ifdef _WIN32
    MSG message;

    while (GetMessage(&message, NULL, 0, 0)) {
      TranslateMessage(&message);
      DispatchMessage(&message);
    }
#endif
#endif
  }

  void ShowCursor() {
#if USE_GLFW
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#endif
  }

  void HideCursor() {
#if USE_GLFW
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
  }

  void setDragCursor() {
#if USE_GLFW
    if (current_cursor == GLFW_IBEAM_CURSOR)
      return;
    GLFWcursor *cursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    glfwSetCursor(mWindow, cursor);
    current_cursor = GLFW_IBEAM_CURSOR;
#endif
  }

  void setNormalCursor() {
#if USE_GLFW
    if (current_cursor == GLFW_ARROW_CURSOR)
      return;
    glfwSetCursor(mWindow, nullptr);
    current_cursor = GLFW_ARROW_CURSOR;
#endif
  }

  void setFullScreen() {
    auto mainMonitorMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowMonitor(mWindow, glfwGetPrimaryMonitor(), 0, 0, mWidth, mHeight, mainMonitorMode->refreshRate);

    mWindowMode = WindowMode::FULLSCREEN;
  }

  void setBorderlessFullscreen() {
    auto mainMonitorMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    glfwSetWindowAttrib(mWindow, GLFW_DECORATED, GLFW_FALSE);
    glfwSetWindowSize(mWindow, mDefaultWidth, mDefaultHeight);
    glfwSetWindowPos(mWindow, 0, 0);

    mLastWidth = mWidth;
    mLastHeight = mHeight;
    mWidth = mDefaultWidth;
    mHeight = mDefaultHeight;

    glfwSetWindowMonitor(mWindow, nullptr, 0, 0, mDefaultWidth, mDefaultHeight, mainMonitorMode->refreshRate);
    mWindowMode = WindowMode::BORDERLESS;
  }

  void setWindowed() {
    auto mainMonitorMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    glfwSetWindowMonitor(mWindow, nullptr, 0, 30, mainMonitorMode->width, mainMonitorMode->height - 30, mainMonitorMode->refreshRate);
    glfwSetWindowPos(mWindow, 0, 30);
    glfwSetWindowAttrib(mWindow, GLFW_DECORATED, GLFW_TRUE);
    mWindowMode = WindowMode::WINDOWED;
  }

  WindowMode getWindowMode() { return mWindowMode; }

  int current_cursor = 0;

  int defaultSizeIndex = 0;

  void setCursorPosition(int x, int y) {
#if USE_GLFW
    glfwSetCursorPos(mWindow, x, y);
#endif
  }

  WindowSize getSize() {
#if USE_GLFW
    if (mWindow == nullptr)
      return {0, 0};

    // glfwGetFramebufferSize(mWindow, &mWidth, &mHeight);
    return {(uint16_t)mWidth, (uint16_t)mHeight};
#endif
#if defined(__ANDROID__)

    return {640, 480};
#endif
  }

  void resize(int w, int h) {

#if USE_GLFW
    glfwSetWindowSize(mWindow, w, h);
    mLastWidth = mWidth;
    mLastHeight = mHeight;
    mWidth = w;
    mHeight = h;
#endif
  }

  int getDefaultSizeIndex() { return defaultSizeIndex; }

  void cleanup() {
#if USE_GLFW
    glfwDestroyWindow(mWindow);
    glfwTerminate();
#endif
  }

  void setWindowTitle(const char *title) {
#if USE_GLFW
    glfwSetWindowTitle(mWindow, title);
#endif
  }

  void setWindowIcon(const char *icon_path) {
    GLFWimage images[3] = {};
    int channels = 4;
    images[0].pixels = framework::load_image_from_file("./media/icon_48.png", &images[0].width, &images[0].height, &channels);
    images[1].pixels = framework::load_image_from_file("./media/icon_32.png", &images[1].width, &images[1].height, &channels);
    images[2].pixels = framework::load_image_from_file("./media/icon_16.png", &images[2].width, &images[2].height, &channels);
    glfwSetWindowIcon(mWindow, 3, images);
    framework::free_image_data(images[0].pixels);
    framework::free_image_data(images[1].pixels);
    framework::free_image_data(images[2].pixels);

    const char *description;
    int code = glfwGetError(&description);

    if (description)
      Log::Error("Setting window icon: \nCode: ", code, "\nDescription: ", description);
  }

  windowType *getWindow() const { return mWindow; }

  void setWindow(windowType *window) { mWindow = window; }

  const std::vector<WindowSizeDescription> &getAvailableSizes() { return m_pAvailableSizes; }

  bool SizeChanged() {
    bool changed = mWidth != mLastWidth || mHeight != mLastHeight;
    mLastWidth = mWidth;
    mLastHeight = mHeight;
    return changed;
  }
  static void window_size_callback(GLFWwindow *window, int width, int height) {
    mWidth = width;
    mHeight = height;
  }

private:
  static int mWidth;
  static int mHeight;

  int mLastWidth = 640;
  int mLastHeight = 480;

  WindowMode mWindowMode = WindowMode::WINDOWED;

  int mDefaultWidth = 1;
  int mDefaultHeight = 1;
  std::vector<WindowSizeDescription> m_pAvailableSizes;

  windowType *mWindow;
#if defined(_WIN32) && (USE_GLFW == false)
  ATOM WINAPI RegisterClassEx(const WNDCLASSEX *lpwcx);
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
      DestroyWindow(hWnd);
      PostQuitMessage(0);
      break;
    case WM_PAINT:
      ValidateRect(hWnd, NULL);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
    }
  }
  HINSTANCE windowInstance;
#endif
};
} // namespace framework