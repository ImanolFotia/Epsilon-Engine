#pragma once

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
#include <jni.h>
#include <android/native_window.h>
#endif


#if USE_GLFW
#include <GLFW/glfw3.h>
#endif


#if defined(_WIN32)
//  Microsoft
#if BUILD_SHARED_LIBS
#if defined(EPSILON_BUILD_DLL)
#define EPSILON_DLL __declspec(dllexport)
#else
#define EPSILON_DLL __declspec(dllimport)
#endif
#elif defined(__linux__)
//  GCC
#if defined(EPSILON_BUILD_DLL)
#define EPSILON_DLL __attribute__((visibility("default")))
#else
#define EPSILON_DLL
#endif
#else
//  do nothing and hope for the best?
#define EPSILON_DLL
#define EPSILON_DLL
#pragma warning Unknown dynamic link import / export semantics.
#endif
#else
#define EPSILON_DLL
#endif

namespace framework
{
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

	class EPSILON_DLL Window
	{



	public:

		enum class WindowMode {
			WINDOWED = 0,
			FULLSCREEN,
			BORDERLESS
		};
#if USE_GLFW
		using windowType = GLFWwindow;
#elif _WIN32
		using windowType = HWND;
#elif defined(__ANDROID__)
		using windowType = ANativeWindow;
#endif
	public:
		void init(std::string appName, int w = 1280, int h = 720);

		void mainLoop()
		{
#if USE_GLFW
			while (!glfwWindowShouldClose(mWindow))
			{
				glfwPollEvents();
			}
#else
#ifdef _WIN32

#endif
#endif
		}

		bool ShouldClose()
		{
#if USE_GLFW
			return glfwWindowShouldClose(mWindow);
#endif
			return false;
		}

		void PollEvents()
		{
#if USE_GLFW
			glfwPollEvents();
#else
#ifdef _WIN32
			MSG message;

			while (GetMessage(&message, NULL, 0, 0))
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
#endif
#endif
		}

		void ShowCursor()
		{
#if USE_GLFW
			glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#endif
		}

		void HideCursor()
		{
#if USE_GLFW
			glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
		}

		void setDragCursor() {
#if USE_GLFW
			if (current_cursor == GLFW_RESIZE_NESW_CURSOR) return;
			GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
			glfwSetCursor(mWindow, cursor);
			current_cursor = GLFW_RESIZE_NESW_CURSOR;
#endif

		}

		void setNormalCursor() {
#if USE_GLFW
			if (current_cursor == GLFW_ARROW_CURSOR) return;
			glfwSetCursor(mWindow, NULL);
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

			glfwSetWindowMonitor(mWindow, NULL, 0, 0, mDefaultWidth, mDefaultHeight, mainMonitorMode->refreshRate);
			mWindowMode = WindowMode::BORDERLESS;
		}

		void setWindowed() {
			auto mainMonitorMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

			glfwSetWindowMonitor(mWindow, NULL, 0, 30, mainMonitorMode->width, mainMonitorMode->height - 30, mainMonitorMode->refreshRate);
			glfwSetWindowPos(mWindow, 0, 30);
			glfwSetWindowAttrib(mWindow, GLFW_DECORATED, GLFW_TRUE);
			mWindowMode = WindowMode::WINDOWED;
		}

		WindowMode getWindowMode() {
			return mWindowMode;
		}

		int current_cursor = 0;

		int defaultSizeIndex = 0;

		void setCursorPosition(int x, int y)
		{
#if USE_GLFW
			glfwSetCursorPos(mWindow, x, y);
#endif
		}

		WindowSize getSize()
		{
#if USE_GLFW
			if (mWindow == nullptr) return { 0,0 };

			glfwGetFramebufferSize(mWindow, &mWidth, &mHeight);
			return { (uint16_t)mWidth, (uint16_t)mHeight };
#endif
#if defined(__ANDROID__)

			return { 640, 480 };
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

		int getDefaultSizeIndex() {
			return defaultSizeIndex;
		}

		void cleanup()
		{
#if USE_GLFW
			glfwDestroyWindow(mWindow);
			glfwTerminate();
#endif
		}

		void setWindowTitle(const char* title)
		{
#if USE_GLFW
			glfwSetWindowTitle(mWindow, title);
#endif
		}

		windowType* getWindow() const
		{
			return mWindow;
		}

		void setWindow(windowType* window)
		{
			mWindow = window;
		}

		const std::vector<WindowSizeDescription>& getAvailableSizes() {
			return m_pAvailableSizes;
		}

		bool SizeChanged() {
			bool changed = mWidth != mLastWidth || mHeight != mLastHeight;
			mLastWidth = mWidth;
			mLastHeight = mHeight;
			return changed;
		}
	private:
		int mWidth = 640;
		int mHeight = 480;


		int mLastWidth = 640;
		int mLastHeight = 480;

		WindowMode mWindowMode = WindowMode::WINDOWED;

		int mDefaultWidth = 1;
		int mDefaultHeight = 1;
		std::vector<WindowSizeDescription> m_pAvailableSizes;

		windowType* mWindow;
#if defined(_WIN32) && (USE_GLFW == false)
		ATOM WINAPI RegisterClassEx(
			const WNDCLASSEX* lpwcx);
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
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
		}