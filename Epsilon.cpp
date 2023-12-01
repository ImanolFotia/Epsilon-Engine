//
// Created by solaire on 2/3/23.
//
#include "Epsilon.hpp"

namespace Epsilon
{
    Epsilon::Epsilon(const std::string &appName)
    {
        m_ApplicationName = appName;
        //engine::Context::getSingleton().Init(appName, engine::renderer_type::vulkan);
        m_pContext = std::make_shared<engine::Context>();
        m_pContext->Init(appName, engine::renderer_type::vulkan);
    }

    std::shared_ptr<engine::Context> Epsilon::getContext()
    {
        if (m_pContext == nullptr) {

            m_pContext = std::make_shared<engine::Context>();
        }

        return m_pContext;
    }

    void Epsilon::run()
    {
        if (m_pContext->Window().getSize().width == 0)
            initWindow(1280, 720);

        initVulkan();
        if (onCreate)
            onCreate();

        if (onReady)
            onReady();

#ifdef __EMSCRIPTEN__

        emscripten_set_main_loop_arg(
            [](void *userData)
            {
                mainLoop();
            },
            nullptr,
            0, true);

#else // __EMSCRIPTEN__

        while (!m_pContext->Window().ShouldClose())
        {
            mainLoop();
            if (mShouldClose)
                break;
        }
#endif
        exit();
    }

    void Epsilon::ShouldClose()
    {
        mShouldClose = true;
    }

    void Epsilon::drawFrame(engine::Ref<engine::RenderPass> renderPassRef)
    {
        glm::vec3 vec;
         m_pContext->Renderer()->Flush(renderPassRef, engine::DrawType::INDEXED);
         m_pContext->Renderer()->End(vec);
         m_pContext->Renderer()->Submit();
         m_pContext->Renderer()->EndFrame();
    }

    void Epsilon::setOnCreate(std::function<void(void)> fun)
    {
        onCreate = fun;
    }

    void Epsilon::setOnReady(std::function<void(void)> fun)
    {
        onReady = fun;
    }

    void Epsilon::setOnRender(std::function<void(void)> fun)
    {
        onRender = fun;
    }

    void Epsilon::setOnUpdate(std::function<void(void)> fun)
    {
        onUpdate = fun;
    }

    void Epsilon::setOnExit(std::function<void(void)> fun)
    {
        onExit = fun;
    }

    void Epsilon::initWindow(int w, int h)
    {
        m_pContext->Window().init(m_ApplicationName, w, h);
    }

    void Epsilon::initVulkan()
    {
        m_pContext->Renderer()->Init(m_ApplicationName.c_str(), m_pContext->Window());
        m_pContext->ResourceManager()->Init();
    }

    void Epsilon::mainLoop()
    {
        double llastTime = framework::Clock::Now();
        framework::Clock::Tick();
        showFPS();
        if (onUpdate)
            onUpdate();

        if (onRender)
            onRender();
        m_pContext->Window().PollEvents();
        m_pFrame++;
        
        double max_fps = (double)m_pMaxFPS;
        double min_frametime = 1000000.0 / max_fps;

        double accum =0;

        while (accum < min_frametime) {
            double currentTime = framework::Clock::Now();
            accum += currentTime - llastTime;
            llastTime = currentTime;
        }
    }

    void Epsilon::showFPS()
    {
        // Measure speed
        double currentTime = framework::Clock::TimeSeconds();
        double delta = currentTime - lastTime;
        nbFrames++;
        if (delta >= 1.0)
        {
            double fps = double(nbFrames) / delta;

            std::stringstream ss;
            ss << m_ApplicationName << " | "
               << " [" << (int)fps << " FPS] | [" << (1000.0 / double(nbFrames)) << " MS]";

            m_pContext->Window().setWindowTitle(ss.str().c_str());

            nbFrames = 0;
            lastTime = currentTime;
        }
    }

    int32_t Epsilon::Frame()
    {
        return m_pFrame;
    }

    void Epsilon::exit()
    {
        if (onExit)
            onExit();
         m_pContext->Renderer()->Cleanup();
         m_pContext->Window().cleanup();
         m_pContext->CleanUp();
    }

    

    inline framework::WindowSize Epsilon::getWindowDimensions()
    {
        int w, h;
        return  m_pContext->Window().getSize();
    }
}

#if BUILD_AS_LIBRARY == TRUE



#ifndef BUILD_ANDROID
#include <apis/vk/vk.hpp>

int lib_test()
{
    return 0;
}
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    static Epsilon::Epsilon instance;

    EXPORT void EpsilonInit(const char* name)
    {
        std::cout << "Creating epsilon instance: " << name << std::endl;
        //auto &instance = Epsilon::Epsilon::getSingleton();
        instance.m_ApplicationName = name;
    }

#ifdef CreateWindow
#undef CreateWindow
#endif

    [[maybe_unused]] EXPORT void CreateWindow(const char* name, int w, int h)
    {
        instance.getContext()->Window().init(name, w, h);
        instance.m_ApplicationName = name;
    }

    EXPORT void CreateContext(const char* name, engine::renderer_type api)
    {
        instance.getContext()->Init(name, api);
    }

    EXPORT void EpsilonRun()
    {
        instance.run();
    }

    EXPORT std::shared_ptr<engine::Context> getContext() {
        return instance.getContext();
    }

    EXPORT void beginScene() {
        instance.getScene()->BeginScene();
    }

    EXPORT void endScene() {
        instance.getScene()->EndScene();
    }


#ifdef __cplusplus
};
#endif



#endif
