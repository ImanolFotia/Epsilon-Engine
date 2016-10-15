#pragma once

#include <vector>

#include <GUI/Control.h>
#include <GUI/Controls/Button.h>
#include <GUI/Controls/Label.h>
#include <GUI/Controls/AlertBox.h>
#include <GUI/Renderers/OpenGL 3.x+/Renderer.h>
#include <GLFW/glfw3.h>

namespace GUI
{

class GUI
{


public:

    /*! Constructor
    @param width Window's Width
    @param height Window's Height
    */

    GUI(int width, int height) : m_WindowWidth(width), m_WindowHeight(height) {}
    /*! Destructor
    */
    //virtual ~GUI() = default;

    /*! Draws all widgets in current GUI
        @param deltaTime Time taken to render one frame*/
    EGUIAPI DrawGUI(float);
    /*! Polls events in current API instance
        @param window Current window*/
    EGUIAPI PollGUIEvents(GLFWwindow*);
    /*! Adds a new Widget to the current Widget container
        @param Widget pointer
        @return true if success, false in failure*/
    bool AddControl(Control*);

    /*! Sets the render to draw the GUI
        @param Renderer pointer*/
    EGUIAPI setRenderer(Renderer::OpenGL3x*);

private:
    /*! Container for all of the Widgets in this GUI instance
    */
    std::vector<Control*> m_ControlContainer;
    int m_WindowWidth;
    int m_WindowHeight;
    Renderer::OpenGL3x* m_Renderer;
    GUIEVENTS Events;
};
}
