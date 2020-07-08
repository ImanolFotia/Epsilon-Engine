#pragma once

#ifdef __cplusplus_cli

#include <windows.h>

#include <Core.hpp>

//TODO
//Class to quicly setup an OpenGL element to use inseide a Windows Forms App
//Further integration is required, and much of this code belongs in the Context class

namespace Epsilon
{
    namespace Platform
    {
        namespace Windows
        {
            using namespace System::Windows::Forms;

            public ref class WindowsFormsComponent : public System::Windows::Forms::Control
            {
            public:
                WindowsFormsComponent(System::Windows::Forms::Form ^ parentForm, GLsizei iWidth, GLsizei iHeight)
                {
                    System::Windows::Forms::CreateParams ^ cp = gcnew System::Windows::Forms::CreateParams;
                    // Set the position on the form
                    cp->X = 0;
                    cp->Y = 0;
                    cp->Height = iHeight;
                    cp->Width = iWidth;
                    mWidth = iWidth;
                    mHeight = iHeight;

                    this->Location = System::Drawing::Point(0, 0);
                    this->Size = System::Drawing::Size(cp->Width, cp->Height);

                    // Specify the form as the parent.
                    cp->Parent = parentForm->Handle;

                    // Create as a child of the specified parent and make OpenGL compliant (no clipping)
                    cp->Style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

                    // Create the actual window
                    this->CreateHandle();

                    m_hDC = GetDC((HWND)this->Handle.ToPointer());

                    if (m_hDC)
                    {
                        MySetPixelFormat(m_hDC);
                        Resize(iWidth, iHeight);
                        InitGL();
                    }

                    rtri = 0.0f;
                    rquad = 0.0f;
                }

                System::Void Render()
                {
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                }

                System::Void SwapOpenGLBuffers(System::Void)
                {
                    SwapBuffers(m_hDC);
                }

                GLvoid Resize(GLsizei width, GLsizei height) // Resize and initialise the gl window
                {
                    if (height == 0) // Prevent A Divide By Zero By
                    {
                        height = 1; // Making Height Equal One
                    }

                    mWidth = width;
                    mHeight = height;
                    this->Size = System::Drawing::Size(mWidth, mHeight);

                    glViewport(0, 0, mWidth, mHeight); // Reset The Current Viewport
                                                       // Reset The Modelview Matrix
                }

            private:
                HDC m_hDC;
                HGLRC m_hglrc;
                GLfloat rtri;  // Angle for the triangle
                GLfloat rquad; // Angle for the quad
                int mWidth, mHeight;

            protected:
                ~WindowsFormsComponent(System::Void)
                {
                    this->DestroyHandle();
                }

                GLint MySetPixelFormat(HDC hdc)
                {
                    static PIXELFORMATDESCRIPTOR pfd = // pfd Tells Windows How We Want Things To Be
                        {
                            sizeof(PIXELFORMATDESCRIPTOR), // Size Of This Pixel Format Descriptor
                            1,                             // Version Number
                            PFD_DRAW_TO_WINDOW |           // Format Must Support Window
                                PFD_SUPPORT_OPENGL |       // Format Must Support OpenGL
                                PFD_DOUBLEBUFFER,          // Must Support Double Buffering
                            PFD_TYPE_RGBA,                 // Request An RGBA Format
                            16,                            // Select Our Color Depth
                            0, 0, 0, 0, 0, 0,              // Color Bits Ignored
                            0,                             // No Alpha Buffer
                            0,                             // Shift Bit Ignored
                            0,                             // No Accumulation Buffer
                            0, 0, 0, 0,                    // Accumulation Bits Ignored
                            16,                            // 16Bit Z-Buffer (Depth Buffer)
                            0,                             // No Stencil Buffer
                            0,                             // No Auxiliary Buffer
                            PFD_MAIN_PLANE,                // Main Drawing Layer
                            0,                             // Reserved
                            0, 0, 0                        // Layer Masks Ignored
                        };

                    GLint iPixelFormat;

                    // get the device context's best, available pixel format match
                    if ((iPixelFormat = ChoosePixelFormat(hdc, &pfd)) == 0)
                    {
                        MessageBox::Show("ChoosePixelFormat Failed");
                        return 0;
                    }

                    // make that match the device context's current pixel format
                    if (SetPixelFormat(hdc, iPixelFormat, &pfd) == FALSE)
                    {
                        MessageBox::Show("SetPixelFormat Failed");
                        return 0;
                    }

                    if ((m_hglrc = wglCreateContext(m_hDC)) == NULL)
                    {
                        MessageBox::Show("wglCreateContext Failed");
                        return 0;
                    }

                    if ((wglMakeCurrent(m_hDC, m_hglrc)) == NULL)
                    {
                        MessageBox::Show("wglMakeCurrent Failed");
                        return 0;
                    }

                    if (!gladLoadGL())
                    {
                        MessageBox::Show("Failed to initialize GLAD");
                        return 0;
                    }

                    return 1;
                }

                bool InitGL(GLvoid) // All setup for opengl goes here
                {
                    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
                    glClearDepth(1.0f);                   // Depth buffer setup
                    glEnable(GL_DEPTH_TEST);              // Enables depth testing
                    glDepthFunc(GL_LEQUAL);               // The type of depth testing to do

                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    return TRUE; // Initialisation went ok
                }
            };
        } // namespace Windows
    }     // namespace Platform
} // namespace Epsilon
#endif