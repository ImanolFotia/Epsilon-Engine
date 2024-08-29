#pragma once 

#include "core/engine/renderer/types.hpp"
#include <glm/glm.hpp>
namespace UI
{

struct Window {
  size_t id = 0;
  glm::vec2 size = glm::vec2(0.0);
  glm::vec2 position{};
  size_t vtx_index = 0;
  bool fixed_size = false;
  glm::vec2 setted_size;
  glm::vec4 color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
  float border_radius = -1.0f;
};

struct Mouse {
  glm::ivec2 position;
  bool left_down = false;
  bool left_was_down = false;
  bool right_down = false;
  bool left_pressed = false;
  bool left_released = false;
  bool right_pressed = false;
  bool right_released = false;
};

struct AngularButtonWidget {
  int subdivisions = 1;
  int buttonCount = 1;
  int currentButtonIndex = 0;
  float angular_step = 3.14159f;
  float innerRadius = 100;
  float outerRadius = 200;
  glm::vec2 position;
};

struct Context {
  glm::ivec2 window_size{};
  glm::ivec2 cursor_position{};
  glm::ivec2 mouse_position{};

  glm::vec2 prev_widget_size{};
  glm::vec2 prev_widget_position{};

  uint32_t pending_actions = 0;
  std::unordered_map<size_t, Window> windows;
  AngularButtonWidget currentAngularWidget;
  bool lastItemHovered = false;
};

struct UIVertex {
  glm::vec4 pos_uv{};
  glm::vec4 color = glm::vec4(0.2, 0.2, 0.2, 0.5);
  glm::vec2 vtx_pos = glm::vec2(-1.0);
  glm::vec2 padding = glm::vec2(-1.0);
};

struct Rect {
  glm::vec2 position;
  glm::vec2 size;
};

struct Style {

  float windowPadding = 5.0f;
  float buttonPadding = 5.0f;
  float buttonHeight = 20;
  glm::vec2 textSize = glm::vec2(10, 20);
  float fontWidth = 12;
};

struct DrawCommand {
  int32_t vertex_offset;
  int32_t num_vertices;
  engine::Ref<engine::BindGroup> bind_group;
};

struct ButtonFlags {
  bool fill_width = false;
  bool center_text = false;
  bool is_tab = false;
  bool is_selected_tab = false;
  float border_radius = -1.0f;
};

enum class DockLocation : uint8_t { TOP = 0, BOTTOM, LEFT, RIGHT };

struct font_pixel {
  uint8_t r, g, b, a;
};
}