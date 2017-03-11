#pragma once
#include <Widget.h>

class GUI{
public:
    GUI(){}
    ~GUI(){}

    void GUIRender();

    int GUIAddWidget(std::shared_ptr<Widget>);

    bool GUIRemoveWidget(int WidgetID);


private:

    std::vector<std::shared_ptr<Widget>> m_WidgetContainer();

};

