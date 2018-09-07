#pragma once
#include <GUI/Widget.h>
#include <Text.h>
#include <string>
#include <vector>
class Console : public Widget{
public:
    Console(int w, int h, int fs, glm::vec4 c): mWidth(w), mHeight(h), mFontSize(fs), mBackgroundColor(c)
    {
        this->m_Quad = (std::shared_ptr<OpenGLHelpers::Quad>) new OpenGLHelpers::Quad((this->mWidth), (this->mHeight));
        this->projection = glm::ortho(0.0f, static_cast<GLfloat>(this->mWidth), 0.0f, static_cast<GLfloat>(this->mHeight));

        this->WIDTH = this->mWidth;
        this->HEIGHT = this->mHeight;
    }

    virtual ~Console(){}

    Render(){}

    Toggle(){}

    AddCommand(){}

    ExecuteCommand(){}

    bool isShowing(){ return this->mShowing; }

private:
    bool mShowing;
    std::vector<std::string> mOutput;
    std::string mInput;
    int mWidth;
    int mHeight;
    int mFontSize;
    glm::vec4 mBackgroundColor;
    glm::mat4 projection;
    std::shared_ptr<OpenGLHelpers::Quad> m_Quad;
protected:

};
