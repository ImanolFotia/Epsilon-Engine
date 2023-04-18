//
// Created by solaire on 2/3/23.
//
#include "Epsilon.hpp"

template <>
Epsilon::Epsilon singleton<Epsilon::Epsilon>::self{};
namespace Epsilon
{
    Epsilon::Epsilon(const std::string &appName)
    {
        self.m_ApplicationName = appName;
        engine::Context::getSingleton().Init(appName, engine::renderer_type::vulkan);
    }

    engine::Context &Epsilon::getContext()
    {
        return engine::Context::getSingleton();
    }

    void Epsilon::run()
    {
        if (engine::Context::getSingleton().Window().getSize().first == 0)
            self.initWindow(1280, 720);

        self.initVulkan();
        if (self.onCreate)
            self.onCreate();

        if (self.onReady)
            self.onReady();
        self.mainLoop();
        self.exit();
    }

    void Epsilon::ShouldClose()
    {
        mShouldClose = true;
    }

    void Epsilon::drawFrame(engine::Ref<engine::RenderPass> renderPassRef)
    {
        glm::vec3 vec;
        engine::Context::getSingleton().Renderer()->Flush(renderPassRef, engine::DrawType::INDEXED);
        engine::Context::getSingleton().Renderer()->End(vec);
        engine::Context::getSingleton().Renderer()->Submit();
        engine::Context::getSingleton().Renderer()->EndFrame();
    }

    void Epsilon::setOnCreate(std::function<void(void)> fun)
    {
        Epsilon::getSingleton().onCreate = fun;
    }

    void Epsilon::setOnReady(std::function<void(void)> fun)
    {
        Epsilon::getSingleton().onReady = fun;
    }

    void Epsilon::setOnRender(std::function<void(void)> fun)
    {
        Epsilon::getSingleton().onRender = fun;
    }

    void Epsilon::setOnExit(std::function<void(void)> fun)
    {
        Epsilon::getSingleton().onExit = fun;
    }

    void Epsilon::initWindow(int w, int h)
    {
        engine::Context::getSingleton().Window().init(m_ApplicationName, w, h);
    }

    void Epsilon::initVulkan()
    {
        engine::Context::getSingleton().Renderer()->Init(m_ApplicationName.c_str(), engine::Context::getSingleton().Window());
        engine::Context::getSingleton().ResourceManager()->Init();
    }

    void Epsilon::mainLoop()
    {

        while (!engine::Context::getSingleton().Window().ShouldClose())
        {
            framework::Clock::Tick();
            showFPS();
            if (mShouldClose)
                break;
            if (self.onRender)
                self.onRender();
            engine::Context::getSingleton().Window().PollEvents();
            self.m_pFrame++;
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

            engine::Context::getSingleton().Window().setWindowTitle(ss.str().c_str());

            nbFrames = 0;
            lastTime = currentTime;
        }
    }

    int32_t Epsilon::Frame() {
        return self.m_pFrame;
    }

    void Epsilon::exit()
    {
        if (self.onExit)
            self.onExit();
        engine::Context::getSingleton().Renderer()->Cleanup();
        engine::Context::getSingleton().Window().cleanup();
    }

    std::pair<int, int> Epsilon::getWindowDimensions()
    {
        int w, h;
        return engine::Context::getSingleton().Window().getSize();
    }
}