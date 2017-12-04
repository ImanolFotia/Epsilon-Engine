#pragma once
#include <GUI/Container.h>
#include <GUI/Panel.h>
#include <GUI/Button.h>

class GUI {
public:
    GUI(int winWidth, int winHeight) : m_WindowWidth(winWidth), m_WindowHeight(winHeight){
        m_Shader = (std::shared_ptr<Shader>) new Shader("shaders/GUI/vertexCommon.glsl", "shaders/GUI/panel.glsl");
    }
    ~GUI() {}

    void Render() {
        for(auto C: m_ContainerCollection) {
            C->Render(m_Shader);
        }
    }

    bool AddContainer(std::shared_ptr<Container> container) {
        if(container == nullptr) {
            return false;
        }
        m_ContainerCollection.push_back(container);

    }

    bool GUIRemoveWidget(int WidgetID);

    void PollEvents(GLFWwindow* window)
    {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        Events.MousePosition[0] = (x / this->m_WindowWidth) * 2 - 1;
        Events.MousePosition[1] = (y / this->m_WindowHeight) * 2 - 1;
        Events.MousePosition[1] *= -1;

        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            Events.LeftClickWasPressed = true;
        else
            Events.LeftClickWasPressed = false;

        for(unsigned int i = 0; i < m_ContainerCollection.size(); ++i)
        {
            m_ContainerCollection[i]->AnalizeEvents(Events);
        }
    }

private:

    std::vector<std::shared_ptr<Container>> m_ContainerCollection;
    std::shared_ptr<Shader> m_Shader;
    GUIEVENTS Events;
    int m_WindowWidth;
    int m_WindowHeight;

};

