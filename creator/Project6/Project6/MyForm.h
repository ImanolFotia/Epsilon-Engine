#pragma once

//#include "OpenGL.h"

#include <Platform/Windows/WindowsFormsComponent.hpp>
#include <Driver/API/OpenGL/HelperFunctions/FullScreenQuad.h>

#include <Shader.h>

#include <memory>

namespace EpsilonCreator {

	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void);

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}

			delete Quad;
		}
	private: System::Windows::Forms::MenuStrip^ menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^ fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ exitToolStripMenuItem;
	private: Epsilon::Platform::Windows::WindowsFormsComponent^ cOpenGL;
	private: System::Windows::Forms::Timer^ MainLoopTimer;

	private: System::Windows::Forms::Panel^ PanelGL;

	private: System::Windows::Forms::StatusStrip^ statusStrip1;
	private: System::Windows::Forms::Panel^ panel2;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::TreeView^ treeView1;
	private: System::Windows::Forms::Panel^ panel3;
	private: System::Windows::Forms::Label^ lblProperties;
	private: System::Windows::Forms::Button^ button1;
	private: System::Windows::Forms::Button^ button2;
	private: System::Windows::Forms::PropertyGrid^ propertyGrid1;
	private: System::Windows::Forms::ToolStripMenuItem^ editToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ viewToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ aboutToolStripMenuItem;
	private: System::Windows::Forms::Panel^ panel1;
	private: System::Windows::Forms::ToolStripMenuItem^ newToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ openToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ saveToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ saveAsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ settingsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ cameraToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ renderingToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ projectToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ compileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ compileAndRunToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ deployToolStripMenuItem;

	private:
		Epsilon::OpenGLHelpers::FullScreenQuad *Quad;
		Epsilon::Shader* shader;





	private: System::ComponentModel::IContainer^ components;
	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->newToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveAsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->editToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->settingsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->viewToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->cameraToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->renderingToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->projectToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->compileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->compileAndRunToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->deployToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->MainLoopTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->PanelGL = (gcnew System::Windows::Forms::Panel());
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->treeView1 = (gcnew System::Windows::Forms::TreeView());
			this->panel3 = (gcnew System::Windows::Forms::Panel());
			this->propertyGrid1 = (gcnew System::Windows::Forms::PropertyGrid());
			this->lblProperties = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->menuStrip1->SuspendLayout();
			this->panel2->SuspendLayout();
			this->panel3->SuspendLayout();
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->BackColor = System::Drawing::SystemColors::ControlDark;
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {
				this->fileToolStripMenuItem,
					this->editToolStripMenuItem, this->viewToolStripMenuItem, this->projectToolStripMenuItem, this->aboutToolStripMenuItem
			});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->RenderMode = System::Windows::Forms::ToolStripRenderMode::Professional;
			this->menuStrip1->Size = System::Drawing::Size(991, 24);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {
				this->newToolStripMenuItem,
					this->openToolStripMenuItem, this->saveToolStripMenuItem, this->saveAsToolStripMenuItem, this->exitToolStripMenuItem
			});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
			this->fileToolStripMenuItem->Text = L"File";
			// 
			// newToolStripMenuItem
			// 
			this->newToolStripMenuItem->Name = L"newToolStripMenuItem";
			this->newToolStripMenuItem->Size = System::Drawing::Size(121, 22);
			this->newToolStripMenuItem->Text = L"New";
			// 
			// openToolStripMenuItem
			// 
			this->openToolStripMenuItem->Name = L"openToolStripMenuItem";
			this->openToolStripMenuItem->Size = System::Drawing::Size(121, 22);
			this->openToolStripMenuItem->Text = L"Open";
			// 
			// saveToolStripMenuItem
			// 
			this->saveToolStripMenuItem->Name = L"saveToolStripMenuItem";
			this->saveToolStripMenuItem->Size = System::Drawing::Size(121, 22);
			this->saveToolStripMenuItem->Text = L"Save";
			// 
			// saveAsToolStripMenuItem
			// 
			this->saveAsToolStripMenuItem->Name = L"saveAsToolStripMenuItem";
			this->saveAsToolStripMenuItem->Size = System::Drawing::Size(121, 22);
			this->saveAsToolStripMenuItem->Text = L"Save as...";
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->Size = System::Drawing::Size(121, 22);
			this->exitToolStripMenuItem->Text = L"Exit";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &MyForm::exitToolStripMenuItem_Click);
			// 
			// editToolStripMenuItem
			// 
			this->editToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->settingsToolStripMenuItem });
			this->editToolStripMenuItem->Name = L"editToolStripMenuItem";
			this->editToolStripMenuItem->Size = System::Drawing::Size(39, 20);
			this->editToolStripMenuItem->Text = L"Edit";
			// 
			// settingsToolStripMenuItem
			// 
			this->settingsToolStripMenuItem->Name = L"settingsToolStripMenuItem";
			this->settingsToolStripMenuItem->Size = System::Drawing::Size(116, 22);
			this->settingsToolStripMenuItem->Text = L"Settings";
			// 
			// viewToolStripMenuItem
			// 
			this->viewToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->cameraToolStripMenuItem,
					this->renderingToolStripMenuItem
			});
			this->viewToolStripMenuItem->Name = L"viewToolStripMenuItem";
			this->viewToolStripMenuItem->Size = System::Drawing::Size(44, 20);
			this->viewToolStripMenuItem->Text = L"View";
			// 
			// cameraToolStripMenuItem
			// 
			this->cameraToolStripMenuItem->Name = L"cameraToolStripMenuItem";
			this->cameraToolStripMenuItem->Size = System::Drawing::Size(128, 22);
			this->cameraToolStripMenuItem->Text = L"Camera";
			// 
			// renderingToolStripMenuItem
			// 
			this->renderingToolStripMenuItem->Name = L"renderingToolStripMenuItem";
			this->renderingToolStripMenuItem->Size = System::Drawing::Size(128, 22);
			this->renderingToolStripMenuItem->Text = L"Rendering";
			// 
			// projectToolStripMenuItem
			// 
			this->projectToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
				this->compileToolStripMenuItem,
					this->compileAndRunToolStripMenuItem, this->deployToolStripMenuItem
			});
			this->projectToolStripMenuItem->Name = L"projectToolStripMenuItem";
			this->projectToolStripMenuItem->Size = System::Drawing::Size(56, 20);
			this->projectToolStripMenuItem->Text = L"Project";
			// 
			// compileToolStripMenuItem
			// 
			this->compileToolStripMenuItem->Name = L"compileToolStripMenuItem";
			this->compileToolStripMenuItem->Size = System::Drawing::Size(166, 22);
			this->compileToolStripMenuItem->Text = L"Compile";
			// 
			// compileAndRunToolStripMenuItem
			// 
			this->compileAndRunToolStripMenuItem->Name = L"compileAndRunToolStripMenuItem";
			this->compileAndRunToolStripMenuItem->Size = System::Drawing::Size(166, 22);
			this->compileAndRunToolStripMenuItem->Text = L"Compile and Run";
			// 
			// deployToolStripMenuItem
			// 
			this->deployToolStripMenuItem->Name = L"deployToolStripMenuItem";
			this->deployToolStripMenuItem->Size = System::Drawing::Size(166, 22);
			this->deployToolStripMenuItem->Text = L"Deploy";
			// 
			// aboutToolStripMenuItem
			// 
			this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
			this->aboutToolStripMenuItem->Size = System::Drawing::Size(52, 20);
			this->aboutToolStripMenuItem->Text = L"About";
			// 
			// MainLoopTimer
			// 
			this->MainLoopTimer->Enabled = true;
			this->MainLoopTimer->Interval = 10;
			this->MainLoopTimer->Tick += gcnew System::EventHandler(this, &MyForm::MainLoop);
			// 
			// PanelGL
			// 
			this->PanelGL->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->PanelGL->Dock = System::Windows::Forms::DockStyle::Fill;
			this->PanelGL->Location = System::Drawing::Point(202, 24);
			this->PanelGL->Name = L"PanelGL";
			this->PanelGL->Size = System::Drawing::Size(587, 531);
			this->PanelGL->TabIndex = 1;
			this->PanelGL->SizeChanged += gcnew System::EventHandler(this, &MyForm::PanelGL_SizeChanged);
			this->PanelGL->Resize += gcnew System::EventHandler(this, &MyForm::PanelGL_Resize);
			// 
			// statusStrip1
			// 
			this->statusStrip1->Location = System::Drawing::Point(0, 555);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(991, 22);
			this->statusStrip1->TabIndex = 2;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// panel2
			// 
			this->panel2->AutoSize = true;
			this->panel2->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->panel2->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			this->panel2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel2->Controls->Add(this->label1);
			this->panel2->Controls->Add(this->treeView1);
			this->panel2->Dock = System::Windows::Forms::DockStyle::Left;
			this->panel2->Location = System::Drawing::Point(0, 24);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(202, 531);
			this->panel2->TabIndex = 3;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->ForeColor = System::Drawing::SystemColors::ControlLightLight;
			this->label1->Location = System::Drawing::Point(4, 4);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(38, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Scene";
			// 
			// treeView1
			// 
			this->treeView1->BackColor = System::Drawing::SystemColors::InactiveCaptionText;
			this->treeView1->Location = System::Drawing::Point(0, 26);
			this->treeView1->Name = L"treeView1";
			this->treeView1->Size = System::Drawing::Size(197, 268);
			this->treeView1->TabIndex = 0;
			// 
			// panel3
			// 
			this->panel3->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel3->Controls->Add(this->propertyGrid1);
			this->panel3->Controls->Add(this->lblProperties);
			this->panel3->Dock = System::Windows::Forms::DockStyle::Right;
			this->panel3->Location = System::Drawing::Point(789, 24);
			this->panel3->Name = L"panel3";
			this->panel3->Size = System::Drawing::Size(202, 531);
			this->panel3->TabIndex = 4;
			// 
			// propertyGrid1
			// 
			this->propertyGrid1->Location = System::Drawing::Point(3, 290);
			this->propertyGrid1->Name = L"propertyGrid1";
			this->propertyGrid1->Size = System::Drawing::Size(194, 236);
			this->propertyGrid1->TabIndex = 1;
			// 
			// lblProperties
			// 
			this->lblProperties->AutoSize = true;
			this->lblProperties->Location = System::Drawing::Point(3, 274);
			this->lblProperties->Name = L"lblProperties";
			this->lblProperties->Size = System::Drawing::Size(54, 13);
			this->lblProperties->TabIndex = 0;
			this->lblProperties->Text = L"Properties";
			this->lblProperties->Click += gcnew System::EventHandler(this, &MyForm::lblProperties_Click);
			// 
			// button1
			// 
			this->button1->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->button1->Location = System::Drawing::Point(194, 0);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 5;
			this->button1->Text = L"Play";
			this->button1->UseVisualStyleBackColor = true;
			// 
			// button2
			// 
			this->button2->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->button2->Location = System::Drawing::Point(275, 0);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 6;
			this->button2->Text = L"Stop";
			this->button2->UseVisualStyleBackColor = true;
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->button1);
			this->panel1->Controls->Add(this->button2);
			this->panel1->Dock = System::Windows::Forms::DockStyle::Top;
			this->panel1->Location = System::Drawing::Point(202, 24);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(587, 23);
			this->panel1->TabIndex = 7;
			this->panel1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MyForm::panel1_Paint);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			this->ClientSize = System::Drawing::Size(991, 577);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->PanelGL);
			this->Controls->Add(this->panel3);
			this->Controls->Add(this->panel2);
			this->Controls->Add(this->statusStrip1);
			this->Controls->Add(this->menuStrip1);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"MyForm";
			this->Text = L"Epsilon Level Editor";
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->panel2->ResumeLayout(false);
			this->panel2->PerformLayout();
			this->panel3->ResumeLayout(false);
			this->panel3->PerformLayout();
			this->panel1->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void exitToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {
		Application::Exit();
	}


	private: System::Void lblProperties_Click(System::Object^ sender, System::EventArgs^ e) {
	}
private: System::Void panel1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) {
}
private: System::Void PanelGL_Resize(System::Object^ sender, System::EventArgs^ e) {
	Control^ control = safe_cast<Control^>(sender);
	cOpenGL->Resize(control->Size.Width, control->Size.Height);
}
private: System::Void PanelGL_SizeChanged(System::Object^ sender, System::EventArgs^ e) {
	Control^ control = safe_cast<Control^>(sender);
	cOpenGL->Resize(control->Size.Width, control->Size.Height);
}
private: System::Void MainLoop(System::Object^ sender, System::EventArgs^ e);
};
}
