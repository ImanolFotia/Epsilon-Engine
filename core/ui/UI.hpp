#pragma once

#include <glm/glm.hpp>
#include <vector>

#include <core/engine/renderer/renderer.hpp>
#include <core/engine/renderer/resource_manager.hpp>
#include <core/framework/IO/Mouse.hpp>

#include <core/framework/common.hpp>
#include <core/framework/utils/file.hpp>
#include <core/framework/utils/image.hpp>

#include "font.hpp"

// #define STBTT_STATIC
// #define STB_TRUETYPE_IMPLEMENTATION
// #include "../thirdparty/stb_truetype.h"

// #include "Roboto.hpp"
#include "Orbitron-Regular.hpp"
#include "Orbitron_Regular_10.hpp"

namespace UI {
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

struct UI {

private:
  const int DEFAULT_FONT = 0;

  Context m_pContext{};
  framework::Window *m_pWindow;
  Mouse m_pMouse{};
  Style m_pStyle{};

  std::vector<DrawCommand> m_DrawLists;
  std::vector<engine::Ref<engine::Buffer>> m_pVertexBuffer;
  std::vector<UIVertex> m_pVertices;
  std::vector<std::shared_ptr<Font>> m_Fonts;

  std::unordered_map<std::string, glm::vec2> m_WhitePixelPositions;
  std::unordered_map<std::string, engine::Ref<engine::BindGroup>> m_pBindGroups;

  std::shared_ptr<engine::ResourceManager> m_pResourceManager = nullptr;
  std::shared_ptr<engine::Renderer> m_pRenderer = nullptr;

  engine::Ref<engine::Mesh> m_pMesh; // dummy mesh to pass the vertex buffer
  engine::Ref<engine::BindGroup> m_CurrentBindGroup;
  engine::Ref<engine::RenderPass> m_pRenderPass;

  size_t vertices_pushed = 0;
  size_t commands_pushed = 0;

  // uint32_t texture_size = 128;
  // uint32_t highest_character = 0;
  uint32_t pending_actions = 0;
  uint32_t current_font = 0;

  glm::vec2 m_pCursorPosition{};
  glm::vec2 m_pResolution = glm::vec2(1280.0f, 720.0f);
  // glm::vec2 m_WhitePixelPos{};

  float m_pAspectRatio = 1280.0f / 720.0f;

  Window *currentWindow = nullptr;

  engine::PipelineLayout uiLayout;

  engine::ShaderInfo loadShaders() {

    auto vertexCode = utils::readFile("./assets/shaders/ui-vertex.spv");
    auto fragmentCode = utils::readFile("./assets/shaders/ui-fragment.spv");
    engine::ShaderInfo mainShaderInfo = {.stages = {{.entryPoint = "main", .shaderCode = vertexCode, .stage = engine::VERTEX},
                                                    {.entryPoint = "main", .shaderCode = fragmentCode, .stage = engine::FRAGMENT}},
                                         .usedStages = engine::ShaderModuleStage(engine::VERTEX | engine::FRAGMENT),
                                         .name = "UIShader"};

    return mainShaderInfo;
  }

public:
  void InitDefaultFont() {
    std::shared_ptr<Font> default_font = std::make_shared<Orbitron>();
    AddFont(default_font);

    std::shared_ptr<Font> small_font = std::make_shared<Orbitron_Regular>();
    AddFont(small_font);

    m_CurrentBindGroup = m_pBindGroups[default_font->Name()];
  }

  void SetFont(int index) {
    current_font = index;

    m_CurrentBindGroup = m_pBindGroups[m_Fonts[current_font]->Name()];
  }

  int32_t AddFont(std::shared_ptr<Font> font) {
    if (font == nullptr)
      return -1;

    std::vector<font_pixel> pixels;
    pixels.resize(font->Dimensions() * font->Dimensions());
    font->texture_size = font->Dimensions();

    for (int i = 0; i < font->texture_size * font->texture_size; i++) {
      pixels[i] = {255, 255, 255, font->Pixels()[i]};

      if (font->Pixels()[i] == 255 && font->white_pixel.x < 0.1) {
        glm::vec2 tmp = glm::vec2(i % font->texture_size, i / font->texture_size) / 256.0f;
        font->white_pixel = {tmp.x, tmp.y};
      }
    }

    if (font->max_range > 0x7F) {
      font->glyph_index.resize(font->max_range);

      for (int i = 0; i < font->num_chars; i++) {
        int byte_count = 1;
        int index = font->DecodeCodepoint(font->Glyphs()[i].code.c_str(), &byte_count);
        font->glyph_index[index] = i;
      }
    }

    for (int i = 0; i < font->num_chars; i++) {

      int glyph_size = abs(font->Glyphs()[i].quad.y0 - font->Glyphs()[i].quad.y1);

      if (glyph_size > font->highest_character) {
        font->highest_character = glm::clamp(glyph_size, 0, font->FontSize());
      }
    }

    engine::TextureCreationInfo texInfo;
    texInfo.pixels = (unsigned char *)pixels.data();
    texInfo.width = font->texture_size;
    texInfo.height = font->texture_size;
    texInfo.format = engine::TextureFormat::COLOR_RGBA;
    texInfo.generateMipMaps = false;
    texInfo.mipLevels = 1;
    texInfo.size = font->texture_size * font->texture_size * 4;

    m_pBindGroups[font->Name()] = m_pResourceManager->createBindGroup({
        .bindingInfo = {{
            .binding = 2,
            .type = engine::UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER,
            .textureInfo = texInfo,
            .name = font->Name(),
        }},
        .inputs = {},
        .renderPass = "SwapChainPass",
        .name = "UIBindGroup" + std::to_string(m_Fonts.size()),
    });

    m_Fonts.push_back(font);

    return m_Fonts.size() - 1;
  }

  void Init(std::shared_ptr<engine::ResourceManager> resource_manager, std::shared_ptr<engine::Renderer> renderer, framework::Window *window) {
    m_pResourceManager = resource_manager;
    m_pRenderer = renderer;
    m_pWindow = window;

    m_pVertices.resize(10000);

    engine::ShaderInfo mainShaderInfo = loadShaders();

    engine::VertexLayout vertexLayout = {.descriptors = {{engine::XYZW_FLOAT, offsetof(UIVertex, pos_uv)},
                                                         {engine::XYZW_FLOAT, offsetof(UIVertex, color)},
                                                         {engine::XY_FLOAT, offsetof(UIVertex, vtx_pos)},
                                                         {engine::XY_FLOAT, offsetof(UIVertex, padding)}},
                                         .size = sizeof(UIVertex)};

    uiLayout = {
        .shaderInfo = mainShaderInfo,
        .vertexLayout = vertexLayout,
        .cullMode = engine::CullMode::NONE,
        .windingMode = engine::WindingMode::CLOCKWISE,
        .depthWriteEnable = false,
        .depthTestEnable = false,
    };
    m_pVertexBuffer.resize(m_pResourceManager->FramesInFlight());

    Log::Info("Allocating ", m_pResourceManager->FramesInFlight(), " vertex buffers for UI.");

    for (int i = 0; i < m_pResourceManager->FramesInFlight(); i++) {

      m_pVertexBuffer[i] = m_pResourceManager->createMappedVertexBuffer("UIVertexBuffer" + std::to_string(i), {.size = 10000 * sizeof(UIVertex)});
    }

    InitDefaultFont();

    m_pMesh = m_pResourceManager->insertMesh("UIMesh", {.vertexBuffer = m_pVertexBuffer[0], .numVertices = 0});

    m_DrawLists.resize(1);
  }

  void AddRenderTarget(const char *name, uint32_t rto_index, const std::string &renderPass) {

    m_pBindGroups[name] = m_pResourceManager->createBindGroup({
        .bindingInfo = {},
        .inputs = {{.renderPass = renderPass, .index = rto_index, .bindingPoint = 2}},
        .renderPass = "SwapChainPass",
        .name = name,
    });
  }

  void AddImageFromReference(const char *image, const char *name) {

    m_pBindGroups[name] = m_pResourceManager->createBindGroup({
        .bindingInfo = {{
            .binding = 2,
            .type = engine::UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER,
            .texture = image,
            .name = name,
        }},
        .inputs = {},
        .renderPass = "SwapChainPass",
        .name = name,
    });
  }

  void AddImageFromFile(const char *path, const char *name) {
    int w, h, c = 4;
    unsigned char *pixels = framework::load_image_from_file(path, &w, &h, &c);

    for (int i = 0; i < w * h; i++) {
      if (pixels[i] == 255 && pixels[i + 1] == 255 && pixels[i + 2] == 255 && pixels[i + 3] == 255) {
        m_WhitePixelPositions[name] = glm::vec2(i % h, i / w) / glm::vec2(w, h);
        break;
      }
    }

    m_WhitePixelPositions[name] = glm::vec2(0.4, 0.03);

    engine::TextureCreationInfo texInfo;
    texInfo.pixels = (unsigned char *)pixels;
    texInfo.width = w;
    texInfo.height = h;
    texInfo.format = engine::TextureFormat::COLOR_RGBA;
    texInfo.generateMipMaps = false;
    texInfo.mipLevels = 1;
    texInfo.size = w * h * 4;

    m_pBindGroups[name] = m_pResourceManager->createBindGroup({
        .bindingInfo = {{
            .binding = 2,
            .type = engine::UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER,
            .textureInfo = texInfo,
            .name = name,
        }},
        .inputs = {},
        .renderPass = "SwapChainPass",
        .name = name,
    });
    framework::free_image_data(pixels);
  }

  void CheckShader();

  glm::vec2 textSize(const std::string &text);
  glm::vec2 buttonSize(const std::string &text);
  glm::vec2 const &Resolution() { return m_pResolution; }

  void Update();
  void Draw(engine::Ref<engine::RenderPass>);
  void Free();
  bool IsHovered();

  glm::vec2 CursorPosition();
  void CursorPosition(glm::vec2 position);

  glm::vec2 ToScreenCoords(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &proj, const glm::vec2 &resolution);

  void CreateRect(glm::vec2 position, glm::vec2 size, glm::vec2 uv0, glm::vec2 uv1, glm::vec4 color = glm::vec4(0.11, 0.11, 0.11, 0.8),
                  float border_radius = -1.0f);

  void CreatePolygon(std::vector<UIVertex> vertices);

  void Circle(int subdivisions = 20);
  void Square();
  void Triangle();
  void Diamond();

  void SetNextWindowPosition(glm::vec2 position);

  void BeginWindow(const std::string &name, glm::vec2 size = glm::vec2(0.0f), glm::vec4 color = glm::vec4(0.0, 0.0, 0.0, 0.5), float border_radius = -1.0f);

  void EndWindow();

  void Text(const std::string &text, glm::vec4 color = glm::vec4(1.0f), bool standalone = true);
  void Text(const std::string &text, glm::vec2 position, glm::vec4 color = glm::vec4(1.0f), bool standalone = true);

  bool Button(const std::string &text, ButtonFlags flags = {});
  void Sameline();

  void Dock(DockLocation, float thickness, float border_radius);

  void Spacer(glm::vec2 size);

  bool ImageButton(const std::string &, const std::string &texture, glm::vec2 size, glm::vec2 uv0, glm::vec2 uv1, glm::vec4 tint = glm::vec4(1.0f),
                   bool fill_background = false, float border_radius = -1.0f);

  void AngularButtons(glm::vec2 position, float innerRadius, float outerRadius, int count);
  bool AngularButton(const std::string &text);

  bool CheckBox(const std::string &text, bool *state);

  void HealthBar(const std::string &text, glm::vec2 position, float val, float min, float max, glm::vec4 color = glm::vec4(0.0, 1.0, 0.0, 1.0),
                 glm::vec4 backgroundColor = glm::vec4(0.0, 0.0, 0.0, 1.0), float border_radius = -1.0f);

  void Switch(const std::string &text, bool &state) {}

  void Scale(const std::string &text, const std::string &setting_name, int &current, int min, int max, glm::vec2 size);
};
} // namespace UI