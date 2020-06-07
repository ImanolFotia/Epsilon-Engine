#pragma once
#include <GUI/Widget.h>

namespace Epsilon {
class Container
{
public:
    Container(){}

    ~Container(){}

    bool addWidget(std::shared_ptr<Widget> widget)
    {
        if(widget == nullptr){
            return false;
        }
        m_WidgetContainer.push_back(widget);
    }

    void Render(std::shared_ptr<Shader> shader)
    {
        for(auto w: m_WidgetContainer){
            if(w->m_isHidden != true)w->Render(shader);
        }
    }

    void AnalizeEvents(GUIEVENTS Events)
    {
        for(auto w: m_WidgetContainer){
            w->AnalizeEvents(Events);
        }
    }

private:
    std::vector<std::shared_ptr<Widget> > m_WidgetContainer;
};
}