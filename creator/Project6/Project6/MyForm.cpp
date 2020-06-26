#include "MyForm.h"

#include <Engine.hpp>


namespace EpsilonCreator {
	static double time = 0.0;
	MyForm::MyForm(void)
	{
		using namespace Epsilon;

		InitializeComponent();
		//
		//TODO: Add the constructor code here
		//

		Epsilon::Engine::Get().ContextType(Epsilon::API::CONTEXT_TYPE::OGL);

		auto position = this->PanelGL->Size;

		cOpenGL = gcnew Epsilon::Platform::Windows::WindowsFormsComponent(this, position.Width, position.Height);
		this->PanelGL->Controls->Add(this->cOpenGL);

		
		Quad = new OpenGLHelpers::FullScreenQuad();
		shader = new Shader("vertex.glsl", "fragment.glsl");
	}

	System::Void MyForm::MainLoop(System::Object^ sender, System::EventArgs^ e) {
		cOpenGL->Render();
		shader->Use();
		shader->PushUniform("iResolution", glm::vec2(cOpenGL->Width, cOpenGL->Height));
		shader->PushUniform("iTime", (float)time);
		Quad->Render();
		cOpenGL->SwapOpenGLBuffers();
		time += 0.01;
	}
}