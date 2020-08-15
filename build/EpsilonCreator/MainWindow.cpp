#include "MainWindow.h"

#include <Engine.hpp>


Epsilon::Engine Epsilon::Engine::instance;

namespace EpsilonCreator {

	MainWindow::MainWindow(void)
	{
		using Epsilon::Platform::Windows::WindowsFormsComponent;

		Epsilon::Engine::Get().ContextType(Epsilon::API::CONTEXT_TYPE::OGL);
		OGLComponent = gcnew WindowsFormsComponent(this, 200, 200);

		InitializeComponent();
		//
		//TODO: Add the constructor code here
		//

		m_Renderer = new Renderer();

		panel1->Controls->Add(OGLComponent);

		this->Show();
		StartUp^ formStartUp = gcnew StartUp();
		auto result = formStartUp->ShowDialog();

	}

	System::Void MainWindow::MainLoop(System::Object^ sender, System::EventArgs^ e) {
		if (OGLComponent == nullptr) return;
		OGLComponent->Render();

		m_Renderer->OnUpdate();

		m_Renderer->OnRender();

		OGLComponent->SwapOpenGLBuffers();
	}

	System::Void MainWindow::ResizeRenderArea(System::Object^ sender, System::EventArgs^ e) {
		if (OGLComponent == nullptr) return;
		auto obj = safe_cast<Panel^>(sender);
		OGLComponent->Resize(obj->Size.Width, obj->Size.Height);
	}
}