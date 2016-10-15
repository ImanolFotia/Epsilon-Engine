#include <GUI/GUI.h>

namespace GUI
{

EGUIAPI GUI::DrawGUI(float deltaTime)
{
    for(int i = 0; i < m_ControlContainer.size(); ++i)
    {
        if(m_ControlContainer[i]->Hidden != true)
            m_ControlContainer[i]->Show(deltaTime, this->m_Renderer);
    }
}

bool GUI::AddControl(Control* control)
{
    this->m_ControlContainer.push_back(control);
    return true;
}

EGUIAPI GUI::setRenderer(Renderer::OpenGL3x* renderer)
{
    this->m_Renderer = renderer;
}

EGUIAPI GUI::PollGUIEvents(GLFWwindow* window)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    Events.MousePosition[0] = (x / this->m_WindowWidth) * 2.0 - 1.0;
    Events.MousePosition[1] = (y / this->m_WindowHeight) * 2 - 1;
    Events.MousePosition[1] *= -1;


    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        Events.LeftClickWasPressed = true;
    else
        Events.LeftClickWasPressed = false;

    for(int i = 0; i < m_ControlContainer.size(); ++i)
    {
        m_ControlContainer[i]->AnalizeEvents(Events);
    }

}

}
