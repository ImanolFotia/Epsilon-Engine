#include "MainWindow.h"

Epsilon::Engine Epsilon::Engine::instance;

namespace EpsilonCreator {

	MainWindow::MainWindow(void)
	{
		using Epsilon::Platform::Windows::WindowsFormsComponent;

		InitializeComponent();
		//
		//TODO: Add the constructor code here
		//

		Epsilon::Engine::Get().ContextType(Epsilon::API::CONTEXT_TYPE::OGL);

		OGLComponent = gcnew WindowsFormsComponent(this, panel1->Size.Width, panel1->Size.Height);
		panel1->Controls->Add(OGLComponent);
	}

	System::Void MainWindow::MainLoop(System::Object^ sender, System::EventArgs^ e) {
		OGLComponent->Render();
		OGLComponent->SwapOpenGLBuffers();
	}
}