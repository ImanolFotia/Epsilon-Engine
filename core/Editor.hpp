#pragma once

#include "App.hpp"
#include <EditorGUI/Editor.hpp>

namespace Epsilon
{
    class Editor : public App
    {
    public:
        Editor() = default;

        virtual void onCreate() override
        {
            Epsilon::Editor::GUI::Init(mWindow);

            Epsilon::Editor::GUI::WIDTH = reinterpret_cast<short *>(mWindow->Width());
            Epsilon::Editor::GUI::HEIGHT = reinterpret_cast<short *>(mWindow->Height());
        }

        virtual void onRender()
        {
            while (!mWindow->WantsToClose())
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                {
                    Epsilon::Editor::GUI::MainWindow(EntityList);
                    Epsilon::Editor::GUI::ToolbarUI();

                    //ImGui::PopStyleVar();
                    //ImGui::Begin("Canvas", NULL);

                    /*Begin skybox window*/

                    //Epsilon::Editor::GUI::SkySettings(skybox);
                    /*End Skybox window*/

                    /*Begin scene window*/

                    //Epsilon::Editor::GUI::DebugView(PP, mDefaultFrameBuffer, eCamera[mCurrentCamera]);
                    //Epsilon::Editor::GUI::MainViewport(mDefaultFrameBuffer, eCamera[mCurrentCamera]);

                    Epsilon::Editor::GUI::SceneHierarchy(EntityList);

                    Epsilon::Editor::GUI::TextureList();

                    //Epsilon::Editor::GUI::PostprocessSettings(PP);

                    Editor::GUI::EntityProperties();

                    Editor::GUI::Render(mWindow);
                }

                mWindow->SwapBuffers();
            }
        }

        virtual void onExit() = 0;

    private:
        int mWidth = 1280;
        int mHeight = 720;
    };
}