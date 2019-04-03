#ifndef GUI_INITIALIZATION_H_INCLUDED
#define GUI_INITIALIZATION_H_INCLUDED

    std::shared_ptr<GUI> m_GUI = (std::shared_ptr<GUI>) new GUI(this->WIDTH, this->HEIGHT);
    std::shared_ptr<Container> t_Container = (std::shared_ptr<Container>) new Container();
    std::shared_ptr<Panel> t_Panel = (std::shared_ptr<Panel>) new Panel(this->WIDTH, this->HEIGHT);
    std::shared_ptr<Button> t_ButtonQuit = (std::shared_ptr<Button>) new Button(0.15, 0.1, this->WIDTH, this->HEIGHT, "Quit");
    t_ButtonQuit->SizeX = 0.15;
    t_ButtonQuit->SizeY = 0.1;
    t_ButtonQuit->PositionX = -0.75;
    t_ButtonQuit->PositionY = -0.45;
    t_ButtonQuit->OnClickCallback(endgame);
    t_ButtonQuit->m_isHidden = false;
    t_ButtonQuit->m_TextRendererInstance = text;
    t_Container->addWidget(t_ButtonQuit);


    std::shared_ptr<Button> t_ButtonSettings = (std::shared_ptr<Button>) new Button(0.15, 0.1, this->WIDTH, this->HEIGHT, "Settings");
    t_ButtonSettings->SizeX = 0.15;
    t_ButtonSettings->SizeY = 0.1;
    t_ButtonSettings->PositionX = -0.75;
    t_ButtonSettings->PositionY = -0.25;
    t_ButtonSettings->OnClickCallback(settings);
    t_ButtonSettings->m_isHidden = false;
    t_ButtonSettings->m_TextRendererInstance = text;
    t_Container->addWidget(t_ButtonSettings);

    t_Container->addWidget(t_Panel);
    m_GUI->AddContainer(t_Container);

#endif // GUI_INITIALIZATION_H_INCLUDED
