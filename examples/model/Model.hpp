#pragma once
#include <Epsilon.hpp>
#include <core/framework/loaders/model.hpp>

namespace ModelExample
{
    class ModelExample : public Epsilon::Epsilon
    {
        framework::Model m_pModel;

    public:
        ModelExample(const std::string &appName) : Epsilon::Epsilon(appName)
        {
            setOnCreate([this]
                        { onCreate(); });
            setOnReady([this]
                       { onReady(); });
            setOnRender([this]
                        { onRender(); });
            setOnExit([this]
                      { onExit(); });
        }

        void onCreate() {}
        void onReady() {}
        void onRender() {}
        void onExit() {}
    };
}