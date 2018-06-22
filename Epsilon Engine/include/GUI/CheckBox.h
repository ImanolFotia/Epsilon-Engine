#pragma once

#include <GUI/Widget.h>
#include <Text.h>
#include <string>

class CheckBox : public Widget {


public:
    CheckBox(float width, float height, int winWidth, int winHeight, std::string text)
        : m_Width(width), m_Height(height), m_winWidth(winWidth), m_winHeight(winHeight), _text(text) {

        m_Quad = (std::shared_ptr<OpenGLHelpers::Quad>) new OpenGLHelpers::Quad((m_Width*m_winWidth), (m_Height*m_winHeight));

        _checkedIco = (std::shared_ptr<eTexture>) new eTexture("gui/Checked.png");
        _uncheckedIco = (std::shared_ptr<eTexture>) new eTexture("gui/Unchecked.png");

        projection = glm::ortho(0.0f, static_cast<GLfloat>(m_winWidth), 0.0f, static_cast<GLfloat>(m_winHeight));
        m_CheckBoxShader = (std::shared_ptr<Shader>) new Shader("shaders/GUI/vertexCommon.glsl", "shaders/GUI/checkbox.glsl");

        this->WIDTH = m_winWidth;
        this->HEIGHT = m_winHeight;

        this->_checked = true;
        m_OnClickFunc = (void(*)())&func;

    }

    virtual ~CheckBox() {}

    void func()
    {
        try{
            Toggle();
            std::cout << "Checked: " << _checked << std::endl;
        }
        catch(...)
        {
            throw;
        }
    }

    const void Toggle() {
        if(_checked) _checked = false;
        else _checked = true;
    }

    void Render(std::shared_ptr<Shader> shader) {
        m_CheckBoxShader->Use();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUniform2f(glGetUniformLocation(m_CheckBoxShader->getProgramID(), "Position"), (this->PositionX*0.5+0.5)*m_winWidth, (this->PositionY*0.5+0.5)*m_winHeight);
        glUniformMatrix4fv(glGetUniformLocation(m_CheckBoxShader->getProgramID(), "projection"), 1, GL_FALSE, &projection[0][0]);

        if(this->Checked()) {
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(glGetUniformLocation(m_CheckBoxShader->getProgramID(), "texture0"), 0);
            glBindTexture(GL_TEXTURE_2D, _checkedIco->getTextureID());
        } else {
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(glGetUniformLocation(m_CheckBoxShader->getProgramID(), "texture0"), 0);
            glBindTexture(GL_TEXTURE_2D, _uncheckedIco->getTextureID());
        }
        m_Quad->Render();
        m_CheckBoxShader->Free();
        glBindTexture(GL_TEXTURE_2D, 0);
        this->m_TextRendererInstance->RenderText(_text,/**/(this->PositionX*0.5+0.5)-(m_TextRendererInstance->getHorizontalLength(_text, 0.8)), this->PositionY*0.5+0.5, 0.8, glm::vec3(1,1,1));
    }

    void Update() {
    }

    void setText(std::string txt) {
        _text = txt;
    }

    bool Checked() {
        return _checked;
    }

    void Check() {
        _checked = true;
    }

    void Uncheck() {
        _checked = false;
    }


    std::shared_ptr<Text> m_TextRendererInstance;
    static bool _checked;
private:
    std::function<void()> _clickfunc;
    std::shared_ptr<eTexture> _checkedIco;
    std::shared_ptr<eTexture> _uncheckedIco;
    glm::mat4 projection;
    std::shared_ptr<OpenGLHelpers::Quad> m_Quad;
    std::string _text;
    std::shared_ptr<Shader> m_CheckBoxShader;
    float m_Width;
    float m_Height;
    float m_winWidth;
    float m_winHeight;
};

