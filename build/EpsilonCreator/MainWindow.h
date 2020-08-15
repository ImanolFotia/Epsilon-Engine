#pragma once

#include <Platform/Windows/WindowsFormsComponent.hpp>

#include <Engine.hpp>

#include "StartUp.h"
#include "Renderer.hpp"

namespace EpsilonCreator {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MainWindow
	/// </summary>
	public ref class MainWindow : public System::Windows::Forms::Form
	{
	public:
		MainWindow(void);

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainWindow()
		{
			if (components)
			{
				delete components;
			}
			delete m_Renderer;
		}
	private: System::Windows::Forms::Timer^ timer1;
	private: System::Windows::Forms::Panel^ panel1;
	private: Epsilon::Platform::Windows::WindowsFormsComponent^ OGLComponent;
	private: System::Windows::Forms::MenuStrip^ WindowMenu;
	private: System::Windows::Forms::ToolStripMenuItem^ fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ exitToolStripMenuItem;
	private: System::Windows::Forms::StatusStrip^ statusStrip1;
	private: System::Windows::Forms::ToolStrip^ toolStrip1;


	private: System::Windows::Forms::Panel^ panel3;





	private: System::Windows::Forms::Panel^ panel2;

	private: System::Windows::Forms::ToolStripComboBox^ toolStripComboBox1;
	private: System::Windows::Forms::ToolStripSeparator^ toolStripSeparator1;
	private: System::Windows::Forms::ToolStripButton^ toolStripButton1;


	private: System::Windows::Forms::ToolStripButton^ toolStripButton2;
	private: System::Windows::Forms::TreeView^ treeView1;
	private: System::Windows::Forms::PropertyGrid^ propertyGrid1;
	private: System::Windows::Forms::SplitContainer^ LeftPanel;

	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::GroupBox^ groupBox1;

	private: System::Windows::Forms::ToolStripMenuItem^ newToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ openToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ closeSceneToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ saveToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ saveAsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ settingsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ editToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ projectToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ helpToolStripMenuItem;



	private: System::Windows::Forms::Button^ button3;
	private: System::Windows::Forms::Button^ button2;
	private: System::Windows::Forms::ToolStripMenuItem^ compileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ compileAndRunToolStripMenuItem;
	private: System::Windows::Forms::ToolStripStatusLabel^ toolStripStatusLabel1;


	private: System::Windows::Forms::ToolStripSeparator^ toolStripSeparator2;
	private: System::Windows::Forms::ToolStripButton^ toolStripButton3;

	private: System::Windows::Forms::ToolStripButton^ toolStripButton4;
	private: System::Windows::Forms::ToolStripButton^ toolStripButton5;
	private: System::Windows::Forms::ToolStripSeparator^ toolStripSeparator3;

	private: System::Windows::Forms::ToolStripButton^ toolStripButton6;

	private: Renderer* m_Renderer;
	private: System::Windows::Forms::SplitContainer^ RightPanel;

	private: System::Windows::Forms::TabControl^ BottomTabControl;
	private: System::Windows::Forms::TabPage^ OutputTab;
	private: System::Windows::Forms::TabPage^ ConsoleTab;
	private: System::Windows::Forms::TabPage^ tabPage1;
	private: System::Windows::Forms::TabPage^ tabPage2;
	private: System::Windows::Forms::SplitContainer^ splitContainer4;



	private: System::Windows::Forms::Panel^ CentralPanel;


	private: System::Windows::Forms::SplitContainer^ splitContainer5;
	private: System::Windows::Forms::SplitContainer^ splitContainer1;



	protected:
	private: System::ComponentModel::IContainer^ components;

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
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MainWindow::typeid));
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->WindowMenu = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->newToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->closeSceneToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveAsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->settingsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->editToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->projectToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->compileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->compileAndRunToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->toolStripStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->toolStrip1 = (gcnew System::Windows::Forms::ToolStrip());
			this->toolStripComboBox1 = (gcnew System::Windows::Forms::ToolStripComboBox());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->toolStripButton1 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButton2 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButton6 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->toolStripButton3 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButton4 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripButton5 = (gcnew System::Windows::Forms::ToolStripButton());
			this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->LeftPanel = (gcnew System::Windows::Forms::SplitContainer());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->treeView1 = (gcnew System::Windows::Forms::TreeView());
			this->panel3 = (gcnew System::Windows::Forms::Panel());
			this->propertyGrid1 = (gcnew System::Windows::Forms::PropertyGrid());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->splitContainer4 = (gcnew System::Windows::Forms::SplitContainer());
			this->RightPanel = (gcnew System::Windows::Forms::SplitContainer());
			this->CentralPanel = (gcnew System::Windows::Forms::Panel());
			this->splitContainer5 = (gcnew System::Windows::Forms::SplitContainer());
			this->BottomTabControl = (gcnew System::Windows::Forms::TabControl());
			this->OutputTab = (gcnew System::Windows::Forms::TabPage());
			this->ConsoleTab = (gcnew System::Windows::Forms::TabPage());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->WindowMenu->SuspendLayout();
			this->statusStrip1->SuspendLayout();
			this->toolStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->LeftPanel))->BeginInit();
			this->LeftPanel->Panel2->SuspendLayout();
			this->LeftPanel->SuspendLayout();
			this->panel2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer4))->BeginInit();
			this->splitContainer4->Panel1->SuspendLayout();
			this->splitContainer4->Panel2->SuspendLayout();
			this->splitContainer4->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->RightPanel))->BeginInit();
			this->RightPanel->Panel1->SuspendLayout();
			this->RightPanel->Panel2->SuspendLayout();
			this->RightPanel->SuspendLayout();
			this->CentralPanel->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer5))->BeginInit();
			this->splitContainer5->Panel1->SuspendLayout();
			this->splitContainer5->Panel2->SuspendLayout();
			this->splitContainer5->SuspendLayout();
			this->BottomTabControl->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->BeginInit();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			this->SuspendLayout();
			// 
			// timer1
			// 
			this->timer1->Enabled = true;
			this->timer1->Interval = 10;
			this->timer1->Tick += gcnew System::EventHandler(this, &MainWindow::MainLoop);
			// 
			// panel1
			// 
			this->panel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel1->Location = System::Drawing::Point(0, 0);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(814, 375);
			this->panel1->TabIndex = 0;
			this->panel1->SizeChanged += gcnew System::EventHandler(this, &MainWindow::ResizeRenderArea);
			// 
			// WindowMenu
			// 
			this->WindowMenu->ImageScalingSize = System::Drawing::Size(32, 32);
			this->WindowMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {
				this->fileToolStripMenuItem,
					this->editToolStripMenuItem, this->projectToolStripMenuItem, this->helpToolStripMenuItem
			});
			this->WindowMenu->Location = System::Drawing::Point(0, 0);
			this->WindowMenu->Name = L"WindowMenu";
			this->WindowMenu->Size = System::Drawing::Size(1194, 24);
			this->WindowMenu->TabIndex = 1;
			this->WindowMenu->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(7) {
				this->newToolStripMenuItem,
					this->openToolStripMenuItem, this->closeSceneToolStripMenuItem, this->saveAsToolStripMenuItem, this->saveToolStripMenuItem, this->settingsToolStripMenuItem,
					this->exitToolStripMenuItem
			});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
			this->fileToolStripMenuItem->Text = L"File";
			// 
			// newToolStripMenuItem
			// 
			this->newToolStripMenuItem->Name = L"newToolStripMenuItem";
			this->newToolStripMenuItem->Size = System::Drawing::Size(136, 22);
			this->newToolStripMenuItem->Text = L"New";
			// 
			// openToolStripMenuItem
			// 
			this->openToolStripMenuItem->Name = L"openToolStripMenuItem";
			this->openToolStripMenuItem->Size = System::Drawing::Size(136, 22);
			this->openToolStripMenuItem->Text = L"Open";
			// 
			// closeSceneToolStripMenuItem
			// 
			this->closeSceneToolStripMenuItem->Name = L"closeSceneToolStripMenuItem";
			this->closeSceneToolStripMenuItem->Size = System::Drawing::Size(136, 22);
			this->closeSceneToolStripMenuItem->Text = L"Close scene";
			// 
			// saveAsToolStripMenuItem
			// 
			this->saveAsToolStripMenuItem->Name = L"saveAsToolStripMenuItem";
			this->saveAsToolStripMenuItem->Size = System::Drawing::Size(136, 22);
			this->saveAsToolStripMenuItem->Text = L"Save as...";
			// 
			// saveToolStripMenuItem
			// 
			this->saveToolStripMenuItem->Name = L"saveToolStripMenuItem";
			this->saveToolStripMenuItem->Size = System::Drawing::Size(136, 22);
			this->saveToolStripMenuItem->Text = L"Save";
			// 
			// settingsToolStripMenuItem
			// 
			this->settingsToolStripMenuItem->Name = L"settingsToolStripMenuItem";
			this->settingsToolStripMenuItem->Size = System::Drawing::Size(136, 22);
			this->settingsToolStripMenuItem->Text = L"Settings";
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->Size = System::Drawing::Size(136, 22);
			this->exitToolStripMenuItem->Text = L"Exit";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainWindow::exitToolStripMenuItem_Click);
			// 
			// editToolStripMenuItem
			// 
			this->editToolStripMenuItem->Name = L"editToolStripMenuItem";
			this->editToolStripMenuItem->Size = System::Drawing::Size(39, 20);
			this->editToolStripMenuItem->Text = L"Edit";
			// 
			// projectToolStripMenuItem
			// 
			this->projectToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
				this->compileToolStripMenuItem,
					this->compileAndRunToolStripMenuItem
			});
			this->projectToolStripMenuItem->Name = L"projectToolStripMenuItem";
			this->projectToolStripMenuItem->Size = System::Drawing::Size(56, 20);
			this->projectToolStripMenuItem->Text = L"Project";
			// 
			// compileToolStripMenuItem
			// 
			this->compileToolStripMenuItem->Name = L"compileToolStripMenuItem";
			this->compileToolStripMenuItem->Size = System::Drawing::Size(163, 22);
			this->compileToolStripMenuItem->Text = L"Compile";
			// 
			// compileAndRunToolStripMenuItem
			// 
			this->compileAndRunToolStripMenuItem->Name = L"compileAndRunToolStripMenuItem";
			this->compileAndRunToolStripMenuItem->Size = System::Drawing::Size(163, 22);
			this->compileAndRunToolStripMenuItem->Text = L"Compile and run";
			// 
			// helpToolStripMenuItem
			// 
			this->helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
			this->helpToolStripMenuItem->Size = System::Drawing::Size(44, 20);
			this->helpToolStripMenuItem->Text = L"Help";
			// 
			// statusStrip1
			// 
			this->statusStrip1->ImageScalingSize = System::Drawing::Size(32, 32);
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->toolStripStatusLabel1 });
			this->statusStrip1->Location = System::Drawing::Point(0, 559);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(1194, 22);
			this->statusStrip1->TabIndex = 2;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// toolStripStatusLabel1
			// 
			this->toolStripStatusLabel1->Name = L"toolStripStatusLabel1";
			this->toolStripStatusLabel1->Size = System::Drawing::Size(39, 17);
			this->toolStripStatusLabel1->Text = L"Ready";
			// 
			// toolStrip1
			// 
			this->toolStrip1->ImageScalingSize = System::Drawing::Size(32, 32);
			this->toolStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(10) {
				this->toolStripComboBox1,
					this->toolStripSeparator1, this->toolStripButton1, this->toolStripButton2, this->toolStripButton6, this->toolStripSeparator2,
					this->toolStripButton3, this->toolStripButton4, this->toolStripButton5, this->toolStripSeparator3
			});
			this->toolStrip1->Location = System::Drawing::Point(0, 24);
			this->toolStrip1->Name = L"toolStrip1";
			this->toolStrip1->Size = System::Drawing::Size(1194, 39);
			this->toolStrip1->TabIndex = 3;
			this->toolStrip1->Text = L"toolStrip1";
			// 
			// toolStripComboBox1
			// 
			this->toolStripComboBox1->Name = L"toolStripComboBox1";
			this->toolStripComboBox1->Size = System::Drawing::Size(121, 39);
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(6, 39);
			// 
			// toolStripButton1
			// 
			this->toolStripButton1->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton1->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9));
			this->toolStripButton1->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"toolStripButton1.Image")));
			this->toolStripButton1->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton1->Name = L"toolStripButton1";
			this->toolStripButton1->Size = System::Drawing::Size(36, 36);
			this->toolStripButton1->Text = L"toolStripButton1";
			// 
			// toolStripButton2
			// 
			this->toolStripButton2->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton2->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"toolStripButton2.Image")));
			this->toolStripButton2->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton2->Name = L"toolStripButton2";
			this->toolStripButton2->Size = System::Drawing::Size(36, 36);
			this->toolStripButton2->Text = L"toolStripButton2";
			// 
			// toolStripButton6
			// 
			this->toolStripButton6->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton6->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"toolStripButton6.Image")));
			this->toolStripButton6->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton6->Name = L"toolStripButton6";
			this->toolStripButton6->Size = System::Drawing::Size(36, 36);
			this->toolStripButton6->Text = L"toolStripButton6";
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->ForeColor = System::Drawing::SystemColors::ActiveCaptionText;
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size(6, 39);
			// 
			// toolStripButton3
			// 
			this->toolStripButton3->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton3->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"toolStripButton3.Image")));
			this->toolStripButton3->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton3->Name = L"toolStripButton3";
			this->toolStripButton3->Size = System::Drawing::Size(36, 36);
			this->toolStripButton3->Text = L"toolStripButton3";
			this->toolStripButton3->Click += gcnew System::EventHandler(this, &MainWindow::toolStripButton3_Click);
			// 
			// toolStripButton4
			// 
			this->toolStripButton4->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton4->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"toolStripButton4.Image")));
			this->toolStripButton4->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton4->Name = L"toolStripButton4";
			this->toolStripButton4->Size = System::Drawing::Size(36, 36);
			this->toolStripButton4->Text = L"toolStripButton4";
			// 
			// toolStripButton5
			// 
			this->toolStripButton5->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
			this->toolStripButton5->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"toolStripButton5.Image")));
			this->toolStripButton5->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->toolStripButton5->Name = L"toolStripButton5";
			this->toolStripButton5->Size = System::Drawing::Size(36, 36);
			this->toolStripButton5->Text = L"toolStripButton5";
			// 
			// toolStripSeparator3
			// 
			this->toolStripSeparator3->Name = L"toolStripSeparator3";
			this->toolStripSeparator3->Size = System::Drawing::Size(6, 39);
			// 
			// LeftPanel
			// 
			this->LeftPanel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->LeftPanel->Location = System::Drawing::Point(0, 0);
			this->LeftPanel->Name = L"LeftPanel";
			this->LeftPanel->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// LeftPanel.Panel2
			// 
			this->LeftPanel->Panel2->Controls->Add(this->button3);
			this->LeftPanel->Panel2->Controls->Add(this->button2);
			this->LeftPanel->Panel2->Controls->Add(this->groupBox1);
			this->LeftPanel->Size = System::Drawing::Size(161, 496);
			this->LeftPanel->SplitterDistance = 239;
			this->LeftPanel->TabIndex = 1;
			// 
			// button3
			// 
			this->button3->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->button3->Location = System::Drawing::Point(59, 3);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(68, 34);
			this->button3->TabIndex = 2;
			this->button3->Text = L"Remove";
			this->button3->UseVisualStyleBackColor = true;
			// 
			// button2
			// 
			this->button2->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Center;
			this->button2->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->button2->Location = System::Drawing::Point(8, 3);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(45, 34);
			this->button2->TabIndex = 1;
			this->button2->Text = L"Add";
			this->button2->UseVisualStyleBackColor = true;
			// 
			// groupBox1
			// 
			this->groupBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBox1->Location = System::Drawing::Point(4, 43);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(157, 207);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Components";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 6);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(84, 16);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Scene Node";
			// 
			// treeView1
			// 
			this->treeView1->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->treeView1->Location = System::Drawing::Point(0, 25);
			this->treeView1->Name = L"treeView1";
			this->treeView1->Size = System::Drawing::Size(211, 223);
			this->treeView1->TabIndex = 0;
			// 
			// panel3
			// 
			this->panel3->AutoSize = true;
			this->panel3->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->panel3->Location = System::Drawing::Point(37, 522);
			this->panel3->Name = L"panel3";
			this->panel3->Size = System::Drawing::Size(0, 0);
			this->panel3->TabIndex = 5;
			// 
			// propertyGrid1
			// 
			this->propertyGrid1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->propertyGrid1->Location = System::Drawing::Point(0, 0);
			this->propertyGrid1->Name = L"propertyGrid1";
			this->propertyGrid1->Size = System::Drawing::Size(211, 244);
			this->propertyGrid1->TabIndex = 0;
			// 
			// panel2
			// 
			this->panel2->Controls->Add(this->splitContainer4);
			this->panel2->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel2->Location = System::Drawing::Point(0, 63);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(1194, 496);
			this->panel2->TabIndex = 7;
			// 
			// splitContainer4
			// 
			this->splitContainer4->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer4->Location = System::Drawing::Point(0, 0);
			this->splitContainer4->Name = L"splitContainer4";
			// 
			// splitContainer4.Panel1
			// 
			this->splitContainer4->Panel1->Controls->Add(this->LeftPanel);
			// 
			// splitContainer4.Panel2
			// 
			this->splitContainer4->Panel2->Controls->Add(this->splitContainer1);
			this->splitContainer4->Panel2->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &MainWindow::splitContainer4_Panel2_Paint);
			this->splitContainer4->Size = System::Drawing::Size(1194, 496);
			this->splitContainer4->SplitterDistance = 161;
			this->splitContainer4->TabIndex = 8;
			// 
			// RightPanel
			// 
			this->RightPanel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->RightPanel->Location = System::Drawing::Point(0, 0);
			this->RightPanel->Name = L"RightPanel";
			this->RightPanel->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// RightPanel.Panel1
			// 
			this->RightPanel->Panel1->BackColor = System::Drawing::SystemColors::Control;
			this->RightPanel->Panel1->Controls->Add(this->label1);
			this->RightPanel->Panel1->Controls->Add(this->treeView1);
			// 
			// RightPanel.Panel2
			// 
			this->RightPanel->Panel2->Controls->Add(this->propertyGrid1);
			this->RightPanel->Size = System::Drawing::Size(211, 496);
			this->RightPanel->SplitterDistance = 248;
			this->RightPanel->TabIndex = 2;
			// 
			// CentralPanel
			// 
			this->CentralPanel->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->CentralPanel->Controls->Add(this->splitContainer5);
			this->CentralPanel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->CentralPanel->Location = System::Drawing::Point(0, 0);
			this->CentralPanel->Name = L"CentralPanel";
			this->CentralPanel->Size = System::Drawing::Size(814, 496);
			this->CentralPanel->TabIndex = 1;
			// 
			// splitContainer5
			// 
			this->splitContainer5->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer5->Location = System::Drawing::Point(0, 0);
			this->splitContainer5->Name = L"splitContainer5";
			this->splitContainer5->Orientation = System::Windows::Forms::Orientation::Horizontal;
			// 
			// splitContainer5.Panel1
			// 
			this->splitContainer5->Panel1->Controls->Add(this->panel1);
			// 
			// splitContainer5.Panel2
			// 
			this->splitContainer5->Panel2->BackColor = System::Drawing::SystemColors::ButtonShadow;
			this->splitContainer5->Panel2->Controls->Add(this->BottomTabControl);
			this->splitContainer5->Size = System::Drawing::Size(814, 496);
			this->splitContainer5->SplitterDistance = 375;
			this->splitContainer5->TabIndex = 0;
			// 
			// BottomTabControl
			// 
			this->BottomTabControl->Controls->Add(this->OutputTab);
			this->BottomTabControl->Controls->Add(this->ConsoleTab);
			this->BottomTabControl->Controls->Add(this->tabPage1);
			this->BottomTabControl->Controls->Add(this->tabPage2);
			this->BottomTabControl->Dock = System::Windows::Forms::DockStyle::Fill;
			this->BottomTabControl->Location = System::Drawing::Point(0, 0);
			this->BottomTabControl->Name = L"BottomTabControl";
			this->BottomTabControl->SelectedIndex = 0;
			this->BottomTabControl->Size = System::Drawing::Size(814, 117);
			this->BottomTabControl->TabIndex = 0;
			// 
			// OutputTab
			// 
			this->OutputTab->Location = System::Drawing::Point(4, 22);
			this->OutputTab->Name = L"OutputTab";
			this->OutputTab->Padding = System::Windows::Forms::Padding(3);
			this->OutputTab->Size = System::Drawing::Size(806, 91);
			this->OutputTab->TabIndex = 0;
			this->OutputTab->Text = L"Output";
			// 
			// ConsoleTab
			// 
			this->ConsoleTab->Location = System::Drawing::Point(4, 22);
			this->ConsoleTab->Name = L"ConsoleTab";
			this->ConsoleTab->Padding = System::Windows::Forms::Padding(3);
			this->ConsoleTab->Size = System::Drawing::Size(628, 170);
			this->ConsoleTab->TabIndex = 1;
			this->ConsoleTab->Text = L"Console";
			// 
			// tabPage1
			// 
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(628, 170);
			this->tabPage1->TabIndex = 2;
			this->tabPage1->Text = L"Materials";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// tabPage2
			// 
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(628, 170);
			this->tabPage2->TabIndex = 3;
			this->tabPage2->Text = L"Models";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// splitContainer1
			// 
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->Location = System::Drawing::Point(0, 0);
			this->splitContainer1->Name = L"splitContainer1";
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->Controls->Add(this->CentralPanel);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->RightPanel);
			this->splitContainer1->Size = System::Drawing::Size(1029, 496);
			this->splitContainer1->SplitterDistance = 814;
			this->splitContainer1->TabIndex = 3;
			// 
			// MainWindow
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1194, 581);
			this->Controls->Add(this->panel2);
			this->Controls->Add(this->panel3);
			this->Controls->Add(this->toolStrip1);
			this->Controls->Add(this->statusStrip1);
			this->Controls->Add(this->WindowMenu);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->MainMenuStrip = this->WindowMenu;
			this->Name = L"MainWindow";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Epsilon Creator";
			this->WindowState = System::Windows::Forms::FormWindowState::Maximized;
			this->Load += gcnew System::EventHandler(this, &MainWindow::MainWindow_Load);
			this->WindowMenu->ResumeLayout(false);
			this->WindowMenu->PerformLayout();
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			this->toolStrip1->ResumeLayout(false);
			this->toolStrip1->PerformLayout();
			this->LeftPanel->Panel2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->LeftPanel))->EndInit();
			this->LeftPanel->ResumeLayout(false);
			this->panel2->ResumeLayout(false);
			this->splitContainer4->Panel1->ResumeLayout(false);
			this->splitContainer4->Panel2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer4))->EndInit();
			this->splitContainer4->ResumeLayout(false);
			this->RightPanel->Panel1->ResumeLayout(false);
			this->RightPanel->Panel1->PerformLayout();
			this->RightPanel->Panel2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->RightPanel))->EndInit();
			this->RightPanel->ResumeLayout(false);
			this->CentralPanel->ResumeLayout(false);
			this->splitContainer5->Panel1->ResumeLayout(false);
			this->splitContainer5->Panel2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer5))->EndInit();
			this->splitContainer5->ResumeLayout(false);
			this->BottomTabControl->ResumeLayout(false);
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->EndInit();
			this->splitContainer1->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void MainLoop(System::Object^ sender, System::EventArgs^ e);

	private: System::Void exitToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e) {

		Application::Exit();
	}
private: System::Void MainWindow_Load(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void splitContainer1_Panel1_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) {
}

private: System::Void ResizeRenderArea(System::Object^ sender, System::EventArgs^ e);
private: System::Void toolStripButton3_Click(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void toolStripLabel1_Click(System::Object^ sender, System::EventArgs^ e) {
}

private: System::Void splitContainer4_Panel2_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e) {
}
};
}
