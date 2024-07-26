#pragma once

#include <imgui.h>

namespace Editor::Catppuccin {

class Catppuccin {
public:
  ImGuiStyle GetStylePalette() { return m_Style; }

  ImVec4 Rosewater()  { return m_Rosewater; }
  ImVec4 Flamingo()   { return m_Flamingo;  }
  ImVec4 Pink()       { return m_Pink;      }
  ImVec4 Mauve()      { return m_Mauve;     }
  ImVec4 Red()        { return m_Red;       }
  ImVec4 Maroon()     { return m_Maroon;    }
  ImVec4 Peach()      { return m_Peach;     }
  ImVec4 Yellow()     { return m_Yellow;    }
  ImVec4 Green()      { return m_Green;     }
  ImVec4 Teal()       { return m_Teal;      }
  ImVec4 Sky()        { return m_Sky;       }
  ImVec4 Sapphire()   { return m_Sapphire;  }
  ImVec4 Blue()       { return m_Blue;      }
  ImVec4 Lavender()   { return m_Lavender;  }
  ImVec4 Text()       { return m_Text;      }
  ImVec4 Subtext1()   { return m_Subtext1;  }
  ImVec4 Subtext0()   { return m_Subtext0;  }
  ImVec4 Overlay2()   { return m_Overlay2;  }
  ImVec4 Overlay1()   { return m_Overlay1;  }
  ImVec4 Overlay0()   { return m_Overlay0;  }
  ImVec4 Surface2()   { return m_Surface2;  }
  ImVec4 Surface1()   { return m_Surface1;  }
  ImVec4 Surface0()   { return m_Surface0;  }
  ImVec4 Base()       { return m_Base;      }
  ImVec4 Mantle()     { return m_Mantle;    }
  ImVec4 Crust()      { return m_Crust;     }

protected:
  ImVec4 m_Rosewater;
  ImVec4 m_Flamingo;
  ImVec4 m_Pink;
  ImVec4 m_Mauve;
  ImVec4 m_Red;
  ImVec4 m_Maroon;
  ImVec4 m_Peach;
  ImVec4 m_Yellow;
  ImVec4 m_Green;
  ImVec4 m_Teal;
  ImVec4 m_Sky;
  ImVec4 m_Sapphire;
  ImVec4 m_Blue;
  ImVec4 m_Lavender;
  ImVec4 m_Text;
  ImVec4 m_Subtext1;
  ImVec4 m_Subtext0;
  ImVec4 m_Overlay2;
  ImVec4 m_Overlay1;
  ImVec4 m_Overlay0;
  ImVec4 m_Surface2;
  ImVec4 m_Surface1;
  ImVec4 m_Surface0;
  ImVec4 m_Base;
  ImVec4 m_Mantle;
  ImVec4 m_Crust;

  ImGuiStyle m_Style;


  void CreatePalette() {

    m_Style.GrabMinSize = 2.0f;
    m_Style.ScrollbarRounding = 2.0f;
    m_Style.GrabRounding = 2.0f;
    m_Style.FrameRounding = 2.0f;
    m_Style.WindowRounding = 2.0f;
    m_Style.ChildRounding = 2.0f;
    m_Style.PopupRounding = 2.0f;
    m_Style.TabRounding = 2.0f;

    ImVec4* colors = m_Style.Colors;

    colors[ImGuiCol_Text]                   = m_Text;
    colors[ImGuiCol_TextDisabled]           = m_Subtext0;
    colors[ImGuiCol_WindowBg]               = m_Base;
    colors[ImGuiCol_ChildBg]                = m_Mantle;
    colors[ImGuiCol_PopupBg]                = m_Mantle;
    colors[ImGuiCol_Border]                 = m_Mantle;
    colors[ImGuiCol_BorderShadow]           = m_Mantle;
    colors[ImGuiCol_FrameBg]                = m_Surface0;
    colors[ImGuiCol_FrameBgHovered]         = m_Surface1;
    colors[ImGuiCol_FrameBgActive]          = m_Surface2;
    colors[ImGuiCol_TitleBg]                = m_Mantle;
    colors[ImGuiCol_TitleBgActive]          = m_Base;
    colors[ImGuiCol_TitleBgCollapsed]       = m_Crust;
    colors[ImGuiCol_MenuBarBg]              = m_Mantle;
    colors[ImGuiCol_ScrollbarBg]            = m_Surface0;
    colors[ImGuiCol_ScrollbarGrab]          = m_Surface1;
    colors[ImGuiCol_ScrollbarGrabHovered]   = m_Lavender;
    colors[ImGuiCol_ScrollbarGrabActive]    = m_Blue;
    colors[ImGuiCol_CheckMark]              = m_Blue;
    colors[ImGuiCol_SliderGrab]             = m_Sky;
    colors[ImGuiCol_SliderGrabActive]       = m_Lavender;
    colors[ImGuiCol_Button]                 = m_Surface2;
    colors[ImGuiCol_ButtonHovered]          = m_Surface1;
    colors[ImGuiCol_ButtonActive]           = m_Surface0;
    colors[ImGuiCol_Header]                 = m_Surface2;
    colors[ImGuiCol_HeaderHovered]          = m_Surface1;
    colors[ImGuiCol_HeaderActive]           = m_Surface0;
    colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered]       = m_Mantle;
    colors[ImGuiCol_SeparatorActive]        = m_Overlay1;
    colors[ImGuiCol_ResizeGrip]             = m_Lavender;
    colors[ImGuiCol_ResizeGripHovered]      = m_Sapphire;
    colors[ImGuiCol_ResizeGripActive]       = m_Blue;
    colors[ImGuiCol_Tab]                    = Lerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive]              = Lerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused]           = Lerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive]     = Lerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(m_Mauve.x, m_Mauve.y, m_Mauve.z, 0.5);
    colors[ImGuiCol_DockingEmptyBg]         = m_Surface0;
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = m_Surface0;
    colors[ImGuiCol_TableBorderStrong]      = m_Overlay2;   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight]       = m_Overlay0;   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg]             = m_Surface0;
    colors[ImGuiCol_TableRowBgAlt]          = m_Surface1;
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(m_Overlay2.x, m_Overlay2.y, m_Overlay2.z, 0.2);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = m_Sky;
    colors[ImGuiCol_NavWindowingHighlight]  = m_Sapphire;
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(m_Overlay0.x, m_Overlay0.y, m_Overlay0.z, 0.5);;
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(m_Overlay2.x, m_Overlay2.y, m_Overlay2.z, 0.5);
  }

//this is so we don't have to include imgui_internal.h
static inline ImVec4 Lerp(const ImVec4& a, const ImVec4& b, float t)          
{ 
    return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t); 
}

};


class Latte : public Catppuccin {
public:
  Latte() {
    m_Rosewater   = ImVec4(220.0f / 255.f, 138.0f / 255.f, 120.0f / 255.f, 255.0f / 255.f);
    m_Flamingo    = ImVec4(221.0f / 255.f, 120.0f / 255.f, 120.0f / 255.f, 255.0f / 255.f);
    m_Pink        = ImVec4(234.0f / 255.f, 118.0f / 255.f, 203.0f / 255.f, 255.0f / 255.f);
    m_Mauve       = ImVec4(136.0f / 255.f,  57.0f / 255.f, 239.0f / 255.f, 255.0f / 255.f);
    m_Red         = ImVec4(210.0f / 255.f,  15.0f / 255.f,  57.0f / 255.f, 255.0f / 255.f);
    m_Maroon      = ImVec4(230.0f / 255.f,  69.0f / 255.f,  83.0f / 255.f, 255.0f / 255.f);
    m_Peach       = ImVec4(254.0f / 255.f, 100.0f / 255.f,  11.0f / 255.f, 255.0f / 255.f);
    m_Yellow      = ImVec4(223.0f / 255.f, 142.0f / 255.f,  29.0f / 255.f, 255.0f / 255.f);
    m_Green       = ImVec4( 64.0f / 255.f, 160.0f / 255.f,  43.0f / 255.f, 255.0f / 255.f);
    m_Teal        = ImVec4( 23.0f / 255.f, 146.0f / 255.f, 153.0f / 255.f, 255.0f / 255.f);
    m_Sky         = ImVec4(  4.0f / 255.f, 165.0f / 255.f, 229.0f / 255.f, 255.0f / 255.f);
    m_Sapphire    = ImVec4( 32.0f / 255.f, 159.0f / 255.f, 181.0f / 255.f, 255.0f / 255.f);
    m_Blue        = ImVec4( 30.0f / 255.f, 102.0f / 255.f, 245.0f / 255.f, 255.0f / 255.f);
    m_Lavender    = ImVec4(114.0f / 255.f, 135.0f / 255.f, 253.0f / 255.f, 255.0f / 255.f);
    m_Text        = ImVec4( 76.0f / 255.f,  79.0f / 255.f, 105.0f / 255.f, 255.0f / 255.f);
    m_Subtext1    = ImVec4( 92.0f / 255.f,  95.0f / 255.f, 119.0f / 255.f, 255.0f / 255.f);
    m_Subtext0    = ImVec4(108.0f / 255.f, 111.0f / 255.f, 133.0f / 255.f, 255.0f / 255.f);
    m_Overlay2    = ImVec4(124.0f / 255.f, 127.0f / 255.f, 147.0f / 255.f, 255.0f / 255.f);
    m_Overlay1    = ImVec4(140.0f / 255.f, 143.0f / 255.f, 161.0f / 255.f, 255.0f / 255.f);
    m_Overlay0    = ImVec4(156.0f / 255.f, 160.0f / 255.f, 176.0f / 255.f, 255.0f / 255.f);
    m_Surface2    = ImVec4(172.0f / 255.f, 176.0f / 255.f, 190.0f / 255.f, 255.0f / 255.f);
    m_Surface1    = ImVec4(188.0f / 255.f, 192.0f / 255.f, 204.0f / 255.f, 255.0f / 255.f);
    m_Surface0    = ImVec4(204.0f / 255.f, 208.0f / 255.f, 218.0f / 255.f, 255.0f / 255.f);
    m_Base        = ImVec4(239.0f / 255.f, 241.0f / 255.f, 245.0f / 255.f, 255.0f / 255.f);
    m_Mantle      = ImVec4(230.0f / 255.f, 233.0f / 255.f, 239.0f / 255.f, 255.0f / 255.f);
    m_Crust       = ImVec4(220.0f / 255.f, 224.0f / 255.f, 232.0f / 255.f, 255.0f / 255.f);

    CreatePalette();
  }
};

class Frappe : public Catppuccin {
public:
  Frappe() {
    m_Rosewater   = ImVec4(242.0f / 255.f, 213.0f / 255.f, 207.0f / 255.f, 255.0f / 255.f);
    m_Flamingo    = ImVec4(238.0f / 255.f, 190.0f / 255.f, 190.0f / 255.f, 255.0f / 255.f);
    m_Pink        = ImVec4(244.0f / 255.f, 184.0f / 255.f, 228.0f / 255.f, 255.0f / 255.f);
    m_Mauve       = ImVec4(202.0f / 255.f, 158.0f / 255.f, 230.0f / 255.f, 255.0f / 255.f);
    m_Red         = ImVec4(231.0f / 255.f, 130.0f / 255.f, 132.0f / 255.f, 255.0f / 255.f);
    m_Maroon      = ImVec4(234.0f / 255.f, 153.0f / 255.f, 156.0f / 255.f, 255.0f / 255.f);
    m_Peach       = ImVec4(239.0f / 255.f, 159.0f / 255.f, 118.0f / 255.f, 255.0f / 255.f);
    m_Yellow      = ImVec4(229.0f / 255.f, 200.0f / 255.f, 144.0f / 255.f, 255.0f / 255.f);
    m_Green       = ImVec4(166.0f / 255.f, 209.0f / 255.f, 137.0f / 255.f, 255.0f / 255.f);
    m_Teal        = ImVec4(129.0f / 255.f, 200.0f / 255.f, 190.0f / 255.f, 255.0f / 255.f);
    m_Sky         = ImVec4(153.0f / 255.f, 209.0f / 255.f, 219.0f / 255.f, 255.0f / 255.f);
    m_Sapphire    = ImVec4(133.0f / 255.f, 193.0f / 255.f, 220.0f / 255.f, 255.0f / 255.f);
    m_Blue        = ImVec4(140.0f / 255.f, 170.0f / 255.f, 238.0f / 255.f, 255.0f / 255.f);
    m_Lavender    = ImVec4(186.0f / 255.f, 187.0f / 255.f, 241.0f / 255.f, 255.0f / 255.f);
    m_Text        = ImVec4(198.0f / 255.f, 208.0f / 255.f, 245.0f / 255.f, 255.0f / 255.f);
    m_Subtext1    = ImVec4(181.0f / 255.f, 191.0f / 255.f, 226.0f / 255.f, 255.0f / 255.f);
    m_Subtext0    = ImVec4(165.0f / 255.f, 173.0f / 255.f, 206.0f / 255.f, 255.0f / 255.f);
    m_Overlay2    = ImVec4(148.0f / 255.f, 156.0f / 255.f, 187.0f / 255.f, 255.0f / 255.f);
    m_Overlay1    = ImVec4(131.0f / 255.f, 139.0f / 255.f, 167.0f / 255.f, 255.0f / 255.f);
    m_Overlay0    = ImVec4(115.0f / 255.f, 121.0f / 255.f, 148.0f / 255.f, 255.0f / 255.f);
    m_Surface2    = ImVec4( 98.0f / 255.f,  104.0f / 255.f, 128.0f / 255.f, 255.0f / 255.f);
    m_Surface1    = ImVec4( 81.0f / 255.f,  87.0f / 255.f, 109.0f / 255.f, 255.0f / 255.f);
    m_Surface0    = ImVec4( 65.0f / 255.f,  69.0f / 255.f,  89.0f / 255.f, 255.0f / 255.f);
    m_Base        = ImVec4( 48.0f / 255.f,  52.0f / 255.f,  70.0f / 255.f, 255.0f / 255.f);
    m_Mantle      = ImVec4( 41.0f / 255.f,  44.0f / 255.f,  60.0f / 255.f, 255.0f / 255.f);
    m_Crust       = ImVec4( 35.0f / 255.f,  38.0f / 255.f,  52.0f / 255.f, 255.0f / 255.f);

    CreatePalette();
  }
};

class Machiatto : public Catppuccin {
public:
  Machiatto() {
    m_Rosewater   = ImVec4(244.0f / 255.f, 219.0f / 255.f, 214.0f / 255.f, 255.0f / 255.f);
    m_Flamingo    = ImVec4(240.0f / 255.f, 198.0f / 255.f, 198.0f / 255.f, 255.0f / 255.f);
    m_Pink        = ImVec4(245.0f / 255.f, 189.0f / 255.f, 230.0f / 255.f, 255.0f / 255.f);
    m_Mauve       = ImVec4(198.0f / 255.f, 160.0f / 255.f, 246.0f / 255.f, 255.0f / 255.f);
    m_Red         = ImVec4(237.0f / 255.f, 135.0f / 255.f, 150.0f / 255.f, 255.0f / 255.f);
    m_Maroon      = ImVec4(238.0f / 255.f, 153.0f / 255.f, 160.0f / 255.f, 255.0f / 255.f);
    m_Peach       = ImVec4(245.0f / 255.f, 169.0f / 255.f, 127.0f / 255.f, 255.0f / 255.f);
    m_Yellow      = ImVec4(238.0f / 255.f, 212.0f / 255.f, 159.0f / 255.f, 255.0f / 255.f);
    m_Green       = ImVec4(166.0f / 255.f, 218.0f / 255.f, 149.0f / 255.f, 255.0f / 255.f);
    m_Teal        = ImVec4(139.0f / 255.f, 213.0f / 255.f, 202.0f / 255.f, 255.0f / 255.f);
    m_Sky         = ImVec4(145.0f / 255.f, 215.0f / 255.f, 227.0f / 255.f, 255.0f / 255.f);
    m_Sapphire    = ImVec4(125.0f / 255.f, 196.0f / 255.f, 228.0f / 255.f, 255.0f / 255.f);
    m_Blue        = ImVec4(138.0f / 255.f, 173.0f / 255.f, 244.0f / 255.f, 255.0f / 255.f);
    m_Lavender    = ImVec4(183.0f / 255.f, 189.0f / 255.f, 248.0f / 255.f, 255.0f / 255.f);
    m_Text        = ImVec4(202.0f / 255.f, 211.0f / 255.f, 245.0f / 255.f, 255.0f / 255.f);
    m_Subtext1    = ImVec4(184.0f / 255.f, 192.0f / 255.f, 224.0f / 255.f, 255.0f / 255.f);
    m_Subtext0    = ImVec4(165.0f / 255.f, 173.0f / 255.f, 203.0f / 255.f, 255.0f / 255.f);
    m_Overlay2    = ImVec4(147.0f / 255.f, 154.0f / 255.f, 183.0f / 255.f, 255.0f / 255.f);
    m_Overlay1    = ImVec4(128.0f / 255.f, 135.0f / 255.f, 162.0f / 255.f, 255.0f / 255.f);
    m_Overlay0    = ImVec4(110.0f / 255.f, 115.0f / 255.f, 141.0f / 255.f, 255.0f / 255.f);
    m_Surface2    = ImVec4( 91.0f / 255.f,  96.0f / 255.f, 120.0f / 255.f, 255.0f / 255.f);
    m_Surface1    = ImVec4( 73.0f / 255.f,  77.0f / 255.f, 100.0f / 255.f, 255.0f / 255.f);
    m_Surface0    = ImVec4( 54.0f / 255.f,  58.0f / 255.f,  79.0f / 255.f, 255.0f / 255.f);
    m_Base        = ImVec4( 36.0f / 255.f,  39.0f / 255.f,  58.0f / 255.f, 255.0f / 255.f);
    m_Mantle      = ImVec4( 30.0f / 255.f,  32.0f / 255.f,  48.0f / 255.f, 255.0f / 255.f);
    m_Crust       = ImVec4( 24.0f / 255.f,  25.0f / 255.f,  38.0f / 255.f, 255.0f / 255.f);

    CreatePalette();
  }
};

class Mocha : public Catppuccin {
public:
  Mocha() {
    m_Rosewater   = ImVec4(245.0f / 255.f, 224.0f / 255.f, 220.0f / 255.f, 255.0f / 255.f);
    m_Flamingo    = ImVec4(242.0f / 255.f, 205.0f / 255.f, 205.0f / 255.f, 255.0f / 255.f);
    m_Pink        = ImVec4(245.0f / 255.f, 194.0f / 255.f, 231.0f / 255.f, 255.0f / 255.f);
    m_Mauve       = ImVec4(203.0f / 255.f, 166.0f / 255.f, 247.0f / 255.f, 255.0f / 255.f);
    m_Red         = ImVec4(243.0f / 255.f, 139.0f / 255.f, 168.0f / 255.f, 255.0f / 255.f);
    m_Maroon      = ImVec4(235.0f / 255.f, 160.0f / 255.f, 172.0f / 255.f, 255.0f / 255.f);
    m_Peach       = ImVec4(250.0f / 255.f, 179.0f / 255.f, 135.0f / 255.f, 255.0f / 255.f);
    m_Yellow      = ImVec4(249.0f / 255.f, 226.0f / 255.f, 175.0f / 255.f, 255.0f / 255.f);
    m_Green       = ImVec4(166.0f / 255.f, 227.0f / 255.f, 161.0f / 255.f, 255.0f / 255.f);
    m_Teal        = ImVec4(148.0f / 255.f, 226.0f / 255.f, 213.0f / 255.f, 255.0f / 255.f);
    m_Sky         = ImVec4(137.0f / 255.f, 220.0f / 255.f, 235.0f / 255.f, 255.0f / 255.f);
    m_Sapphire    = ImVec4(116.0f / 255.f, 199.0f / 255.f, 236.0f / 255.f, 255.0f / 255.f);
    m_Blue        = ImVec4(137.0f / 255.f, 180.0f / 255.f, 250.0f / 255.f, 255.0f / 255.f);
    m_Lavender    = ImVec4(180.0f / 255.f, 190.0f / 255.f, 254.0f / 255.f, 255.0f / 255.f);
    m_Text        = ImVec4(205.0f / 255.f, 214.0f / 255.f, 244.0f / 255.f, 255.0f / 255.f);
    m_Subtext1    = ImVec4(186.0f / 255.f, 194.0f / 255.f, 222.0f / 255.f, 255.0f / 255.f);
    m_Subtext0    = ImVec4(166.0f / 255.f, 173.0f / 255.f, 200.0f / 255.f, 255.0f / 255.f);
    m_Overlay2    = ImVec4(147.0f / 255.f, 153.0f / 255.f, 178.0f / 255.f, 255.0f / 255.f);
    m_Overlay1    = ImVec4(127.0f / 255.f, 132.0f / 255.f, 156.0f / 255.f, 255.0f / 255.f);
    m_Overlay0    = ImVec4(108.0f / 255.f, 112.0f / 255.f, 134.0f / 255.f, 255.0f / 255.f);
    m_Surface2    = ImVec4( 88.0f / 255.f,  91.0f / 255.f, 112.0f / 255.f, 255.0f / 255.f);
    m_Surface1    = ImVec4( 69.0f / 255.f,  71.0f / 255.f,  90.0f / 255.f, 255.0f / 255.f);
    m_Surface0    = ImVec4( 49.0f / 255.f,  50.0f / 255.f,  68.0f / 255.f, 255.0f / 255.f);
    m_Base        = ImVec4( 30.0f / 255.f,  30.0f / 255.f,  46.0f / 255.f, 255.0f / 255.f);
    m_Mantle      = ImVec4( 24.0f / 255.f,  24.0f / 255.f,  37.0f / 255.f, 255.0f / 255.f);
    m_Crust       = ImVec4( 17.0f / 255.f,  17.0f / 255.f,  27.0f / 255.f, 255.0f / 255.f);

    CreatePalette();
  }
};

template <typename T> static ImGuiStyle GetPalette() {
  ImGuiStyle style;

  Catppuccin theme = T();
  return theme.GetStylePalette();
}
} // namespace Editor::Catppuccin