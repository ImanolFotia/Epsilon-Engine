#pragma once
#include <pch.hpp>
#include <GUI/Widget.h>
#include <OpenGL/HelperFunctions/Quad.h>
#include <Text.h>

namespace Epsilon
{
    class Console : public Widget
    {
    public:
        class Line
        {
        public:
            using Color = glm::vec3;
            Line(std::string content, Color col) : mContent(content), mColor(col) {}

            std::string const getText() { return mContent; }

            Color const getColor() { return mColor; }

        private:
            std::string mContent;
            Color mColor;
        };

        Console(int w, int h, int fs, glm::vec4 c) : mWidth(w), mHeight(h), mFontSize(fs), mBackgroundColor(c)
        {
            this->m_Quad = (std::shared_ptr<OpenGLHelpers::Quad>)new OpenGLHelpers::Quad((this->mWidth), (this->mHeight));
            this->projection = glm::ortho(0.0f, static_cast<GLfloat>(this->mWidth), 0.0f, static_cast<GLfloat>(this->mHeight));

            this->WIDTH = this->mWidth;
            this->HEIGHT = this->mHeight;
        }

        virtual ~Console() {}

        void Render()
        {
            this->RenderFrame();
            this->RenderScrollBar();
        }

        void Toggle() { mShowing = !mShowing; }

        void AddCommand() {}

        void ExecuteCommand() {}

        bool isShowing() { return this->mShowing; }

    private:
        void RenderFrame() {}
        void RenderScrollBar() {}
        void RenderText() {}
        void RenderInputText() {}

    private:
        bool mShowing = false;
        std::vector<Line> mLines; //Console lines to be able to scroll and paint them individually
        std::string mInput;
        int mWidth;
        int mHeight;
        int mFontSize;
        glm::vec4 mBackgroundColor;
        glm::mat4 projection;
        std::shared_ptr<OpenGLHelpers::Quad> m_Quad;

    protected:
    };
} // namespace Epsilon