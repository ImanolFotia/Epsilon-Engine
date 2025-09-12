#pragma once

#include "core/engine/renderer/types.hpp"
#include <glm/glm.hpp>
#include <variant>
namespace UI {

const glm::vec4 WHITE = glm::vec4(1.0, 1.0, 1.0, 1.0);
const glm::vec4 GRAY = glm::vec4(0.5, 0.5, 0.5, 1.0);
const glm::vec4 BLACK = glm::vec4(0.0, 0.0, 0.0, 1.0);
const glm::vec4 RED = glm::vec4(1.0, 0.0, 0.0, 1.0);
const glm::vec4 GREEN = glm::vec4(0.0, 1.0, 0.0, 1.0);
const glm::vec4 BLUE = glm::vec4(0.0, 0.0, 1.0, 1.0);
const glm::vec4 LIGHT_BLUE = glm::vec4(0.1, 0.2, 0.7, 1.0);

struct Window {
  size_t id = 0;
  glm::vec2 size = glm::vec2(0.0);
  glm::vec2 position{};
  size_t vtx_index = 0;
  bool fixed_size = false;
  glm::vec2 setted_size;
  glm::vec4 color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
  float border_radius = -1.0f;
  bool draw_header = false;
  bool new_window = true;
  bool fixed = true;
  std::string name;
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
  bool right_was_down = false;
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

struct Color {
  std::variant<glm::vec4, const char*> color = glm::vec4(1.0f);
  Color() = default;
  Color(const char* c) { color = c; }
  Color(glm::vec4 c) { color = c; }

  auto operator=(const char* c)-> void { color = c; }
  auto operator=(glm::vec4 c)-> void { color = c; }

  auto get() -> glm::vec4 {
    if(color.index() == 0) return std::get<glm::vec4>(color);
    if(color.index() == 1) return toVec4(std::get<const char*>(color));
    return {};
  }

  private:
     auto toVec4(const char* c) -> glm::vec4 {
      if(c == nullptr) return {};
      glm::vec4 output = glm::vec4(1.0);
      for(int i = 1, j = 0; i < strlen(c); i+=2,j++) {
        output[j] = ((fromHex(c[i]) << 4) | fromHex(c[i+1])) / 255.0f;
      }
      return output;
    }

    auto fromHex (char c) -> char {
        c = std::toupper(c);
        if(c >= 48 && c <= 57) return (c - '0');
        if(c >= 65 && c <= 70) return (c - 55);

        assert(false && "character is out of range");
    };
};

template <typename T> struct SplinePoint {
  using type = T;
  T position;
  Color color;
};

struct LinePoint {
  glm::vec2 position;
  glm::vec4 color = glm::vec4(1.0);
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
  glm::vec4 attributes = glm::vec4(0.8);
};

struct Rect {
  glm::vec2 position;
  glm::vec2 size;
};

struct Style {

  glm::vec2 windowPadding = glm::vec2(5.0f);
  float buttonPadding = 5.0f;
  float buttonHeight = 20;
  glm::vec2 textSize = glm::vec2(10, 20);
  float fontWidth = 12;
};

enum class Primitive {
  TRIANGLE = 1,
  LINE = 2,
};

struct DrawCommand {
  int32_t vertex_offset;
  int32_t num_vertices;
  Primitive layout;
  engine::Ref<engine::BindGroup> bind_group;
};

enum class ButtonShape { Rect = 0, Circle, Diamond, Size };

struct ButtonFlags {
  bool fill_width = false;
  bool center_text = false;
  bool is_tab = false;
  bool is_selected_tab = false;
  float border_radius = -1.0f;
  glm::vec4 color = glm::vec4(0.25, 0.25, 0.25, 1.0);
  glm::vec4 hover_color = glm::vec4(0.35, 0.35, 0.35, 1.0);
  ButtonShape shape = ButtonShape::Rect;
};

enum class HealthBarStyle {
  SQUARES,
  LINE,
  THIN_LINE
};

struct HealthBarFlags {
  Color color;
  Color background_color;
  HealthBarStyle style;
  float border_radius = -1.0;
};

enum class SplineType { QUADRATIC_BEZIER = 0, CUBIC_BEZIER, CATMULL_ROM, SIZE };

struct SplineFlags {
  bool drawHandles = true;
  bool drawNormals = false;
  bool drawTangents = false;
  bool mirrorControlPoints = true;
  bool fill = false;
  bool projectToScreen = false;
  glm::mat4 transform = glm::mat4(1.0f);
  SplineType type = SplineType::CUBIC_BEZIER;
};

struct ImageButtonFlags {
  bool fill_background = false;
  Color background_color = "#ffffffff";
  Color image_tint = "#ffffffff";
  float border_radius = -1.0f;
};

enum class DockLocation : uint8_t { TOP = 0, BOTTOM, LEFT, RIGHT };

struct font_pixel {
  uint8_t r, g, b, a;
};
} // namespace UI