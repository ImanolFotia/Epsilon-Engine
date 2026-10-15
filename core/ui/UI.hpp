#pragma once

#include <cmath>
#include <core/engine/renderer/renderer.hpp>
#include <core/engine/renderer/resource_manager.hpp>
#include <core/framework/IO/Mouse.hpp>

#include <core/framework/common.hpp>
#include <core/framework/splines/catmull_rom.hpp>
#include <core/framework/utils/file.hpp>
#include <core/framework/utils/image.hpp>

#include "Animation.hpp"
#include "Types.hpp"
#include "core/framework/loaders/gltf.hpp"
#include "core/framework/splines/cuadratic_bezier.hpp"
#include "core/framework/splines/cubic_bezier.hpp"
#include "font.hpp"

#include <filesystem>
#include <optional>
#include <print>
#include <span>
#include <stack>
#include <vector>

// #define STBTT_STATIC
// #define STB_TRUETYPE_IMPLEMENTATION
// #include "../thirdparty/stb_truetype.h"

// #include "Roboto.hpp"
#include "Orbitron-Regular.hpp"
#include "Orbitron_Regular_10.hpp"
#include "glm/common.hpp"
#include "glm/ext.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/trigonometric.hpp"

namespace UI {

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
  engine::Ref<engine::BindGroup> m_CurrentBindGroup{};
  engine::Ref<engine::RenderPass> m_pRenderPass;

  size_t vertices_pushed = 0;
  size_t commands_pushed = 0;

  // uint32_t texture_size = 128;
  // uint32_t highest_character = 0;
  uint32_t pending_actions = 0;
  uint32_t current_font = 0;

  size_t grabed_window_id = 0;
  int selected_point = -1;

  glm::vec2 m_pCursorPosition{};
  glm::vec2 m_pResolution = glm::vec2(1280.0f, 720.0f);
  glm::vec2 m_pLastResolution{};

  glm::ivec2 lastMouse = {};
  glm::ivec2 lastCanvasMouse = {};
  // glm::vec2 mouse;

  float m_LineWidth = 2.0f;

  const size_t VERTEX_COUNT = 450000;
  // glm::vec2 m_WhitePixelPos{};

  float m_pAspectRatio = 1280.0f / 720.0f;
  bool resolution_changed = false;

  AnimationManager *m_CurrentAnimation = nullptr;

  Window *currentWindow = nullptr;

  size_t window_stack_pointer = 100;
  size_t window_stack[100] = {};

  engine::PipelineLayout uiLayout;

  glm::mat4 viewMatrix = glm::mat4(1.0f);
  glm::mat4 projectionMatrix = glm::mat4(1.0f);
  std::stack<glm::mat4> transforms;

  engine::ShaderInfo loadShaders() {

    auto vertexCode = utils::readFile("./assets/shaders/ui-vertex.spv");
    auto fragmentCode = utils::readFile("./assets/shaders/ui-fragment.spv");
    engine::ShaderInfo mainShaderInfo = {.stages = {{.entryPoint = "main", .shaderCode = vertexCode, .stage = engine::VERTEX},
                                                    {.entryPoint = "main", .shaderCode = fragmentCode, .stage = engine::FRAGMENT}},
                                         .usedStages = engine::ShaderModuleStage(engine::VERTEX | engine::FRAGMENT),
                                         .name = "UIShader"};

    return mainShaderInfo;
  }

  void pUpdateAnimation(size_t offset, size_t num) {
    if (m_CurrentAnimation == nullptr)
      return;

    std::span<UIVertex> span = std::span{m_pVertices}.subspan(offset, num);

    m_CurrentAnimation->Update(span, glm::vec2(1.0), glm::vec2(1.0f));

    m_CurrentAnimation = nullptr;
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

  Style &GetStyle();

  float text_size = 1.6;

  float PixelScale(float pt) {
    auto font = m_Fonts[current_font];
    return (pt / font->font_size) * font->texture_size;
  }

  int32_t AddFont(const std::string &path) {
    auto absolute = std::filesystem::absolute(path);
    if (!std::filesystem::exists(absolute)) {
      Log::Error("Provided font :", absolute, " doesn't exist.");
      return -1;
    }
    auto ext = std::filesystem::path(absolute).extension();
    auto filename = std::filesystem::path(absolute).filename();
    auto parent = std::filesystem::path(absolute).parent_path();

    if (ext == ".json") {
      std::ifstream f(path);
      nlohmann::json json = nlohmann::json::parse(f);

      auto &atlas = json["atlas"];
      auto &metrics = json["metrics"];
      auto &glyphs = json["glyphs"];

      std::shared_ptr<Font> font = std::make_shared<Font>();
      std::vector<unsigned char> &pixels = font->Pixels();
      std::vector<Glyph> &font_glyphs = font->Glyphs();

      font->max_range = 0;
      font->num_chars = 0;

      font->Name(filename);

      uint32_t max_range = 0;
      uint32_t num_glyphs = 0;

      for (auto &glyph : glyphs) {
        uint32_t unicode = glyph["unicode"].get<uint32_t>();
        max_range = std::max(max_range, unicode);
        num_glyphs++;
      }

      font_glyphs.resize(num_glyphs);
      font->msdf = true;
      font->font_size = atlas["size"].get<uint32_t>();
      font->glyph_index.resize(max_range + 1);
      font->highest_character = metrics["lineHeight"].get<float>();
      font->yOrigin = QuadOrigin::Bottom;

      int width = atlas["width"].get<int>();
      int height = atlas["height"].get<int>();

      for (auto &glyph : glyphs) {
        uint32_t unicode = glyph["unicode"].get<uint32_t>();
        font->max_range = std::max(font->max_range, unicode);
        font->glyph_index[unicode] = font->num_chars;

        AlignedQuad quad = {
            glyph["planeBounds"]["left"].get<float>(),  -glyph["planeBounds"]["top"].get<float>(),
            glyph["atlasBounds"]["left"].get<float>(),  glyph["atlasBounds"]["top"].get<float>(),

            glyph["planeBounds"]["right"].get<float>(), -glyph["planeBounds"]["bottom"].get<float>(),
            glyph["atlasBounds"]["right"].get<float>(), glyph["atlasBounds"]["bottom"].get<float>(),
        };
/*
        quad.x0 *= font->font_size;
        quad.x1 *= font->font_size;
        quad.y0 *= font->font_size;
        quad.y1 *= font->font_size;
*/
        quad.s0 /= width;
        quad.s1 /= width;
        quad.t0 /= height;
        quad.t1 /= height;

        quad.t0 = 1.0f - quad.t0;
        quad.t1 = 1.0f - quad.t1;

        std::string code{};
        if (unicode <= 0x7F) {
          code = (char)unicode;
        } else {
          continue;
        }

        font_glyphs[unicode - 32] = {quad, code, glyph["advance"].get<float>()};
        font->num_chars++;
      }

      font->texture_size = width;
      pixels.resize(width * width * 4);
      int w, h, c = 4;

      const std::string image_filename = parent.string() + "/" + filename.stem().string() + ".png";

      unsigned char *image_pixels = framework::load_image_from_file(image_filename.c_str(), &w, &h, &c);

      for (int i = 0; i < w * h * 4; i += 4) {
        pixels[i] = image_pixels[i];
        pixels[i + 1] = image_pixels[i + 1];
        pixels[i + 2] = image_pixels[i + 2];
        pixels[i + 3] = image_pixels[i + 3];
      };
      framework::free_image_data(image_pixels);

      double max_dist = {};

      for (int i = 0; i < w * h * 4; i += 4) {
        glm::vec4 v = glm::vec4(pixels[i], pixels[i + 1], pixels[i + 2], pixels[i + 3]);
        double curr_dist = glm::length(v);

        if (curr_dist > max_dist) {
          max_dist = curr_dist;
          // if (pixels[i] == 255 && pixels[i + 1] == 255 && pixels[i + 2] == 255 && pixels[i + 3] == 255) {
          m_WhitePixelPositions[filename] = glm::vec2(i % h, i / w) / glm::vec2(w, h);
          font->white_pixel = {(float)(i % h) / (float)w, (float)((float)i / w) / (float)h};
        }
      }

      engine::TextureCreationInfo texInfo;
      texInfo.pixels = (unsigned char *)pixels.data();
      texInfo.width = font->texture_size;
      texInfo.height = font->texture_size;
      texInfo.format = engine::TextureFormat::COLOR_RGBA;
      texInfo.generateMipMaps = false;
      texInfo.mipLevels = 1;
      texInfo.filtering = engine::Filtering::LINEAR;
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
    return -1;
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
    texInfo.generateMipMaps = true;
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

    m_pVertices.resize(VERTEX_COUNT);
    /*
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
        };*/
    m_pVertexBuffer.resize(m_pResourceManager->FramesInFlight());

    Log::Info("Allocating ", m_pResourceManager->FramesInFlight(), " vertex buffers for UI.");

    for (int i = 0; i < m_pResourceManager->FramesInFlight(); i++) {

      m_pVertexBuffer[i] = m_pResourceManager->createMappedVertexBuffer("UIVertexBuffer" + std::to_string(i), {.size = VERTEX_COUNT * sizeof(UIVertex)});
    }

    InitDefaultFont();

    m_pMesh = m_pResourceManager->insertMesh("UIMesh", {.vertexBuffer = m_pVertexBuffer[0], .numVertices = 0});

    m_DrawLists.resize(1);

    transforms.push(glm::mat4(1.0f));
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

  void AddImageFromFile(const char *path, const char *name, int mipBase = 0, int mipLevels = 1) {
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
    texInfo.filtering = engine::Filtering::LINEAR;
    texInfo.generateMipMaps = mipLevels > 1;
    texInfo.mipLevels = mipLevels;
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

  void PushTransform(const glm::mat4 &transform);
  void PopTransform();

  void SetLineWidth(float width) { m_LineWidth = width; }

  float GetLineWidth() { return m_LineWidth; }

  glm::vec2 textSize(const std::string &text);
  glm::vec2 textSize(std::string_view text);
  glm::vec2 buttonSize(const std::string &text);
  glm::vec2 const &Resolution() { return m_pResolution; }

  static glm::vec2 MousePosition() { return glm::vec2(framework::Input::Mouse::XPOS, framework::Input::Mouse::YPOS); }

  void SetMatrices(glm::mat4 view, glm::mat4 proj) {
    viewMatrix = view;
    projectionMatrix = proj;
  }

  bool Update();
  void Draw(engine::Ref<engine::RenderPass>);
  void Free();
  bool IsHovered();

  glm::vec2 CursorPosition();
  void CursorPosition(glm::vec2 position);

  glm::vec2 ToScreenCoords(const glm::mat4 &model, bool clamp_to_border = false);

  void CreateRect(glm::vec2 position, glm::vec2 size, glm::vec2 uv0, glm::vec2 uv1, glm::vec4 color = glm::vec4(0.11, 0.11, 0.11, 0.8),
                  float border_radius = -1.0f);

  void CreatePolygon(const UIVertex *vertices, size_t size);
  void CreatePolygon(const std::vector<UIVertex>& vertices);

  void Circle(int subdivisions = 20, float size = 10.0f, glm::vec4 color = glm::vec4(0.5, 0.5, 0.5, 1.0));
  void Square(glm::vec2, glm::vec2, glm::vec4 color = glm::vec4(1.0));
  void Triangle();
  void Diamond();

  bool MouseLeftReleased() { return m_pMouse.left_released; }
  bool MouseLeftPressed() { return m_pMouse.left_pressed; }

  bool MouseRightReleased() { return m_pMouse.right_released; }
  bool MouseRightPressed() { return m_pMouse.right_pressed; }

  void DrawLine(LinePoint &&p0, LinePoint &&p1) {

    auto font = m_Fonts[current_font];
    if (m_DrawLists[commands_pushed].layout != Primitive::LINE) {
      auto &new_command = m_DrawLists.emplace_back();
      commands_pushed++;
      new_command.vertex_offset = vertices_pushed;
      new_command.num_vertices = 0;
      new_command.bind_group = m_CurrentBindGroup;
      new_command.layout = Primitive::LINE;
      // if(m_DrawLists.size() > 1)
      new_command.transform = transforms.top(); // m_DrawLists[m_DrawLists.size() - 2].transform;
    }

    m_pVertices[vertices_pushed].color = glm::vec4(p0.color);
    m_pVertices[vertices_pushed].pos_uv = glm::vec4(p0.position.x / m_pResolution.x, p0.position.y / m_pResolution.y, font->white_pixel.x, font->white_pixel.y);
    m_pVertices[vertices_pushed].padding = glm::vec2(-1.0f);
    m_pVertices[vertices_pushed++].vtx_pos = glm::vec2(0.0, -1);

    m_pVertices[vertices_pushed].color = glm::vec4(p1.color);
    m_pVertices[vertices_pushed].pos_uv = glm::vec4(p1.position.x / m_pResolution.x, p1.position.y / m_pResolution.y, font->white_pixel.x, font->white_pixel.y);
    m_pVertices[vertices_pushed].padding = glm::vec2(-1.0f);
    m_pVertices[vertices_pushed++].vtx_pos = glm::vec2(-1.0f);
    m_DrawLists[commands_pushed].num_vertices += 2;
  }

  void Ellipse(glm::vec2 rad) {
    using namespace glm;
    using v2 = glm::vec2;

    auto p = m_pCursorPosition;

    const float subdivisions = 100;

    const float step = (1.0 / subdivisions) * glm::tau<float>();

    for (int i = 0; i < subdivisions * 0.5; i++) {
      float th0 = step * i;
      float th1 = step * (i + 1);
      v2 p0 = v2(cos(th0), -sin(th0)) * rad + p + v2(rad.x, 0.0);
      v2 p1 = v2(cos(th1), -sin(th1)) * rad + p + v2(rad.x, 0.0);

      DrawLine({.position = p0, .color = RED}, {.position = p1, .color = RED});
    }

    m_pCursorPosition = p;
  }

  template <typename T = glm::vec2> void Spline(std::vector<SplinePoint<T>> &points, SplineFlags flags) {
    auto original_position = m_pCursorPosition;
    switch (flags.type) {
    case SplineType::CATMULL_ROM:
      pCatmullRomSpline(points, flags);
      break;

    case SplineType::QUADRATIC_BEZIER:
      pQuadraticBezierSpline(points, flags);
      break;

    case SplineType::CUBIC_BEZIER:
      pCubicBezierSpline(points, flags);
      break;
    default:
      break;
    }

    m_pCursorPosition = original_position;
  }

  void SetNextWidgetAnimation(AnimationManager *anim_mgr);

  void SetNextWindowPosition(glm::vec2 position);

  std::optional<glm::vec2> GetCurrentWindowPosition() {
    if(currentWindow)
      return currentWindow->position;

    return std::nullopt;
  }

  void BeginWindow(const std::string &name, glm::vec2 size = glm::vec2(0.0f), glm::vec4 color = glm::vec4(0.0, 0.0, 0.0, 0.5), float border_radius = -1.0f,
                   bool header = false, bool fixed = true, bool moves = false);

  void EndWindow();

  bool WindowChanged() { return resolution_changed; }

  template <typename... Args> void Text(std::string_view fmt, Args &&...args) {
    auto str = std::vformat(fmt, std::make_format_args(args...));
    Text(str, m_pCursorPosition, glm::vec4(1.0), true);
  }
  void Text(const std::string &text, glm::vec4 color = glm::vec4(1.0f), bool standalone = true);
  void Text(const std::string &text, glm::vec2 position, glm::vec4 color = glm::vec4(1.0f), bool standalone = true);

  bool Button(const std::string &text, ButtonFlags flags = {});
  void Sameline();

  void Dock(DockLocation, float thickness, float border_radius);

  void Spacer(glm::vec2 size);

  bool ImageButton(const std::string &, const std::string &texture, glm::vec2 size, glm::vec2 uv0, glm::vec2 uv1, ImageButtonFlags flags = {});

  void AngularButtons(glm::vec2 position, float innerRadius, float outerRadius, int count);
  bool AngularButton(const std::string &text);

  bool CheckBox(const std::string &text, bool *state);

  void Spinner(glm::vec2 position, float innerRadius, float outerRadius, float percent, glm::vec4 color = glm::vec4(0.5, 0.5, 0.5, 1.0));

  bool HealthBar(const std::string &text, glm::vec2 position, float val, float min, float max, HealthBarFlags flags);

  void Switch(const std::string &text, bool &state) {}

  void Scale(const std::string &text, const std::string &setting_name, int &current, int min, int max, glm::vec2 size);

  glm::vec2 ToScreenCoords(const glm::vec3 &point, bool clamp_to_border = false) {
    glm::vec4 ndc = projectionMatrix * viewMatrix * glm::vec4(point, 1.0);

    ndc /= ndc.w;
    ndc.y = -1.0 * ndc.y;

    glm::vec2 viewport_space = glm::vec2(ndc.x, ndc.y) * 0.5f + 0.5f;

    if (clamp_to_border)
      viewport_space = glm::clamp(viewport_space, glm::vec2(0.0), glm::vec2(1.0));

    return viewport_space * m_pResolution;
  }

private:
  template <typename T> int control_point_move(std::vector<SplinePoint<T>> &points, const SplineFlags &flags) {
    auto circle_col = [](glm::vec2 m, glm::vec2 p, float r) -> bool { return glm::length(m - p) <= r; };
    auto snap = [](T v, int snap) { return (v / snap) * snap; };
    int hovered_point = -1;
    int point_stride[(int)SplineType::SIZE] = {2, 3, 2};
    glm::vec2 mouse_screen = glm::vec2(m_pMouse.position) / m_pResolution;                               // screen-space to clip-space
    mouse_screen = mouse_screen * 2.0f - 1.0f;                                                           // clip-space to ncd
    mouse_screen = (glm::inverse(transforms.top()) * glm::vec4(mouse_screen, 0.0f, 1.0f)) * 0.5f + 0.5f; // ndc to normalized window coords
    mouse_screen *= m_pResolution;                                                                       // normalized window coords to window coords
    glm::ivec2 imouse_screen = glm::ivec2(mouse_screen);
    for (int i = 0; i < points.size(); i++) {
      auto &p = points[i];
      if (circle_col(mouse_screen, p.position, 12) || selected_point == i) {
        hovered_point = i;
        if (m_pMouse.right_down && selected_point == -1 || selected_point == i) {
          selected_point = i;
          p.internal_position.x += (imouse_screen - lastCanvasMouse).x;
          p.internal_position.y += (imouse_screen - lastCanvasMouse).y;

          p.position = glm::ivec2(p.internal_position / flags.snap_size) * flags.snap_size;

          int mod = (int)point_stride[(int)flags.type];

          int x = i % mod;
          if (flags.mirrorControlPoints && flags.type != SplineType::CATMULL_ROM) {
            if (i % mod != 0 && i > 1) {
              if (x == 1) {
                points[i - 2].internal_position = points[i - 1].position - (points[i].position - points[i - 1].position);
                points[i - 2].position = snap(points[i - 2].internal_position, flags.snap_size);
              }
              if (x == (flags.type == SplineType::QUADRATIC_BEZIER ? 0 : 2)) {
                points[i + 2].internal_position = points[i + 1].position - (points[i].position - points[i + 1].position);
                points[i + 2].position = snap(points[i + 2].internal_position, flags.snap_size);
              }
            } else if (i > 1) {
              points[i - 1].internal_position.x += (imouse_screen - lastCanvasMouse).x;
              points[i + 1].internal_position.x += (imouse_screen - lastCanvasMouse).x;

              points[i - 1].internal_position.y += (imouse_screen - lastCanvasMouse).y;
              points[i + 1].internal_position.y += (imouse_screen - lastCanvasMouse).y;

              points[i + 1].position = snap(points[i + 1].internal_position, flags.snap_size);
              points[i - 1].position = snap(points[i - 1].internal_position, flags.snap_size);
            } else if (i == 0) {
              points[i + 1].internal_position.x += (imouse_screen - lastCanvasMouse).x;
              points[i + 1].internal_position.y += (imouse_screen - lastCanvasMouse).y;
              points[i + 1].position = snap(points[i + 1].internal_position, flags.snap_size);
            }
          }
        }
      }
    }
    return hovered_point;
  }
  template <typename T> void pCatmullRomSpline(std::vector<SplinePoint<T>> &points, SplineFlags flags) {

    auto font = m_Fonts[current_font];

    int hovered_point = -1;

    auto circle_col = [](glm::vec2 m, glm::vec2 p, float r) -> bool { return glm::length(m - p) <= r; };
    auto segment_color = [](SplinePoint<T> &p0, SplinePoint<T> &p1, float t) { return glm::vec4(glm::mix(p0.color.get(), p1.color.get(), t)); };
    auto rot = [](float angle) { return glm::mat2(glm::cos(angle), -glm::sin(angle), glm::sin(angle), glm::cos(angle)); };

    framework::splines::CatmullRom<glm::vec2> spline;
    spline.Loops(false);

    glm::vec2 mouse_screen = glm::vec2(m_pMouse.position) / m_pResolution;                               // screen-space to clip-space
    mouse_screen = mouse_screen * 2.0f - 1.0f;                                                           // clip-space to ncd
    mouse_screen = (glm::inverse(transforms.top()) * glm::vec4(mouse_screen, 0.0f, 1.0f)) * 0.5f + 0.5f; // ndc to normalized window coords
    mouse_screen *= m_pResolution;                                                                       // normalized window coords to window coords
    glm::ivec2 imouse_screen = glm::ivec2(mouse_screen);
    for (int i = 0; i < points.size(); i++) {
      auto &p = points[i];
      if (circle_col(mouse_screen, p.position, 12) || selected_point == i) {
        hovered_point = i;
        if (m_pMouse.right_down && selected_point == -1 || selected_point == i) {
          selected_point = i;
          p.internal_position.x += (imouse_screen - lastCanvasMouse).x;
          p.internal_position.y += (imouse_screen - lastCanvasMouse).y;

          p.position = glm::ivec2(p.internal_position / flags.snap_size) * flags.snap_size;
        }
      }

      spline.addPoint(p.position);
    }
    lastCanvasMouse = imouse_screen;
    if (points.size() == 2 && flags.drawHandles) {

      DrawLine({.position = points[0].position, .color = LIGHT_BLUE}, {.position = points[1].position, .color = LIGHT_BLUE});
    }

    if (points.size() >= 4) {

      const int subdivisions = 50;

      for (int i = 0; i < points.size() - 1; i++) {
        for (int j = 0; j < subdivisions; j++) {
          float t = (float)(j) / (float)subdivisions;
          float t2 = (float)(j + 1) / (float)subdivisions;

          auto &p0 = points[i];
          auto &p1 = points[(i + 1) % points.size()];

          auto sp0 = spline.getPoint(t + i);
          auto sp1 = spline.getPoint(t2 + i - ((subdivisions - 1 == j) ? 1e-5 : 0.0));

          if (i < points.size() - 2 &&
              (points[i].position.x >= m_pResolution.x || points[i].position.y >= m_pResolution.y || points[i].position.x < 0 || points[i].position.y < 0) &&
              (points[i + 1].position.x >= m_pResolution.x || points[i + 1].position.y >= m_pResolution.y || points[i + 1].position.x < 0 ||
               points[i + 1].position.y < 0) &&
              (points[i + 2].position.x >= m_pResolution.x || points[i + 2].position.y >= m_pResolution.y || points[i + 2].position.x < 0 ||
               points[i + 2].position.y < 0)) {
            // i++;
            break;
            ;
          }

          auto g = spline.getGradient(t + i);
          float acc = length(g);
          auto grad = glm::normalize(g);
          auto normal = normalize(rot(glm::radians(90.0f)) * grad);
          if (flags.drawTangents) {
            DrawLine({.position = sp0, .color = glm::vec4(grad, 0.0, 1.0)}, {.position = (sp0 + grad * acc * 0.25), .color = glm::vec4(grad, 1.0, 1.0)});
          }
          if (flags.drawNormals) {
            DrawLine({.position = sp0, .color = glm::vec4(normal, 0.0, 1.0)}, {.position = (sp0 + normal * 80.0), .color = glm::vec4(normal, 1.0, 1.0)});
          }
          DrawLine({.position = sp0, .color = segment_color(p0, p1, t)}, {.position = sp1, .color = segment_color(p0, p1, t2)});
        }
      }
      if (flags.drawHandles) {
        for (int i = 0; i < points.size() - 1; i++) {
          DrawLine({.position = points[i].position, .color = LIGHT_BLUE}, {.position = points[i + 1].position, .color = LIGHT_BLUE});
        }
      }
    }
    if (flags.drawHandles) {
      for (int i = 0; i < points.size(); i++) {
        auto &p = points[i];
        m_pCursorPosition = p.position;
        float color = 1.0;
        int subdivs = 15;
        Circle(subdivs, 12, glm::vec4(0.0, 0.0, 0.0, color));
        Circle(subdivs, i == hovered_point ? 8 : 4, p.color.get());
        // Text("({}, {})", p.position.x, p.position.y);
      }
      if (selected_point == -1 && m_pMouse.right_pressed && points.size() >= 1) {
        T handle_position = T(1.0f);
        handle_position.x = imouse_screen.x;
        handle_position.y = imouse_screen.y;
        points.push_back({.position = handle_position, .color = WHITE});
        points.back().internal_position = handle_position;
        selected_point = points.size() - 1;

      } else if (points.size() == 0 && m_pMouse.right_pressed) {
        T handle_position = T(1.0f);
        handle_position.x = imouse_screen.x;
        handle_position.y = imouse_screen.y;
        points.push_back({.position = handle_position, .color = WHITE});
        points.back().internal_position = handle_position;
        selected_point = points.size() - 1;
      }
    }
  }

  template <typename T> void pQuadraticBezierSpline(std::vector<SplinePoint<T>> &points, SplineFlags flags = {}) {

    auto font = m_Fonts[current_font];

    int hovered_point = -1;

    auto circle_col = [](glm::vec2 m, glm::vec2 p, float r) -> bool { return glm::length(m - p) <= r; };
    auto segment_color = [](SplinePoint<T> &p0, SplinePoint<T> &p1, float t) { return glm::vec4(glm::mix(p0.color.get(), p1.color.get(), t)); };
    auto rot = [](float angle) { return glm::mat2(glm::cos(angle), -glm::sin(angle), glm::sin(angle), glm::cos(angle)); };

    framework::splines::CuadraticBezier<glm::vec2> spline;
    std::vector<UIVertex> fill_vertices;
    spline.Loops(false);
    hovered_point = control_point_move(points, flags);

    glm::vec2 mouse_screen = glm::vec2(m_pMouse.position) / m_pResolution;                               // screen-space to clip-space
    mouse_screen = mouse_screen * 2.0f - 1.0f;                                                           // clip-space to ncd
    mouse_screen = (glm::inverse(transforms.top()) * glm::vec4(mouse_screen, 0.0f, 1.0f)) * 0.5f + 0.5f; // ndc to normalized window coords
    mouse_screen *= m_pResolution;                                                                       // normalized window coords to window coords
    glm::ivec2 imouse_screen = glm::ivec2(mouse_screen);

    for (int i = 0; i < points.size(); i++) {
      auto &p = points[i];
      spline.addPoint(p.position);
    }
    lastCanvasMouse = imouse_screen;
    if (points.size() == 2 && flags.drawHandles) {

      DrawLine({.position = points[0].position, .color = LIGHT_BLUE}, {.position = points[1].position, .color = LIGHT_BLUE});
    }

    if (points.size() >= 3) {

      const int subdivisions = 50;

      if (flags.fill) {

        DrawLine({.position = points[0].position, .color = points[0].color.get()},
                 {.position = points[points.size() - 1].position, .color = points[points.size() - 1].color.get()});
      }
      for (int i = 0; i < points.size() - 2; i += 2) {
        for (int j = 0; j < subdivisions; j++) {
          float t = (float)(j) / (float)subdivisions;
          float t2 = (float)(j + 1) / (float)subdivisions;

          auto &p0 = points[i];
          auto &p1 = points[(i + 2) % points.size()];

          auto sp0 = spline.getPoint(t + i);
          auto sp1 = spline.getPoint(t2 + i - ((subdivisions - 1 == j) ? 1e-5 : 0.0));

          if (flags.fill) {
            fill_vertices.push_back(
                {.pos_uv = glm::vec4(points[0].position.x / m_pResolution.x, points[0].position.y / m_pResolution.y, font->white_pixel.x, font->white_pixel.y),
                 .color = p0.color.get(),
                 .vtx_pos = glm::vec2(-1.0f),
                 .padding = glm::vec2(-1.0f)});
            fill_vertices.push_back({.pos_uv = glm::vec4(sp0.x / m_pResolution.x, sp0.y / m_pResolution.y, font->white_pixel.x, font->white_pixel.y),
                                     .color = p0.color.get(),
                                     .vtx_pos = glm::vec2(-1.0f),
                                     .padding = glm::vec2(-1.0f)});
            fill_vertices.push_back({.pos_uv = glm::vec4(sp1.x / m_pResolution.x, sp1.y / m_pResolution.y, font->white_pixel.x, font->white_pixel.y),
                                     .color = p0.color.get(),
                                     .vtx_pos = glm::vec2(-1.0f),
                                     .padding = glm::vec2(-1.0f)});
          }
          auto g = spline.getGradient(t + i);
          float acc = length(g);
          auto grad = glm::normalize(g);
          auto normal = normalize(rot(glm::radians(90.0f)) * grad);
          if (flags.drawTangents) {
            DrawLine({.position = sp0, .color = glm::vec4(grad, 0.0, 1.0)}, {.position = (sp0 + grad * acc * 0.25), .color = glm::vec4(grad, 1.0, 1.0)});
          }
          if (flags.drawNormals) {
            DrawLine({.position = sp0, .color = glm::vec4(normal, 0.0, 1.0)}, {.position = (sp0 + normal * 80.0), .color = glm::vec4(normal, 1.0, 1.0)});
          }
          DrawLine({.position = sp0, .color = segment_color(p0, p1, t)},
                   {.position = (sp1 + glm::normalize(sp1 - sp0) * 0.0), .color = segment_color(p0, p1, t2)});
        }
      }

      if (flags.drawHandles) {
        for (int i = 0; i < points.size() - 1; i++) {
          DrawLine({.position = points[i].position, .color = LIGHT_BLUE}, {.position = points[i + 1].position, .color = LIGHT_BLUE});
        }
      }
    }
    if (flags.drawHandles) {
      for (int i = 0; i < points.size(); i++) {
        auto &p = points[i];
        m_pCursorPosition = p.position;
        float color = 1.0;
        int subdivs = 15;
        if (i % 2)
          subdivs = 4;
        Circle(subdivs, 12, glm::vec4(0.0, 0.0, 0.0, color));
        Circle(subdivs, i == hovered_point ? 8 : 4, p.color.get());
        // Text("({}, {})", p.position.x, p.position.y);
      }
      if (selected_point == -1 && m_pMouse.right_pressed && points.size() >= 1) {
        T handle_position = T(1.0f);
        handle_position.x = imouse_screen.x;
        handle_position.y = imouse_screen.y;
        points.push_back({.position = handle_position, .color = LIGHT_BLUE});
        points.back().internal_position = handle_position;
        points.push_back({.position = handle_position, .color = WHITE});
        // if (points.size() > 4)
        // if (points.size() == 4)
        //   points.push_back({.position = glm::vec2(m_pMouse.position), .color = LIGHT_BLUE});
        points.back().internal_position = handle_position;
        selected_point = points.size() - 2;

      } else if (points.size() == 0 && m_pMouse.right_pressed) {
        T handle_position = T(1.0f);
        handle_position.x = imouse_screen.x;
        handle_position.y = imouse_screen.y;
        points.push_back({.position = handle_position, .color = WHITE});
        points.back().internal_position = handle_position;
        selected_point = points.size() - 1;
      }
    }

    if (flags.fill) {
      CheckShader();
      CreatePolygon(fill_vertices);
    }
  }

  template <typename T> void pCubicBezierSpline(std::vector<SplinePoint<T>> &points, SplineFlags flags = {}) {

    auto font = m_Fonts[current_font];

    int hovered_point = -1;

    auto circle_col = [](glm::vec2 m, glm::vec2 p, float r) -> bool { return glm::length(m - p) <= r; };
    auto segment_color = [](SplinePoint<T> &p0, SplinePoint<T> &p1, float t) { return glm::vec4(glm::mix(p0.color.get(), p1.color.get(), t)); };
    auto rot = [](float angle) { return glm::mat2(glm::cos(angle), -glm::sin(angle), glm::sin(angle), glm::cos(angle)); };

    framework::splines::CubicBezier<glm::vec2> spline;
    std::vector<UIVertex> fill_vertices;
    spline.Loops(false);

    hovered_point = control_point_move(points, flags);

    glm::vec2 mouse_screen = glm::vec2(m_pMouse.position) / m_pResolution;                               // screen-space to clip-space
    mouse_screen = mouse_screen * 2.0f - 1.0f;                                                           // clip-space to ncd
    mouse_screen = (glm::inverse(transforms.top()) * glm::vec4(mouse_screen, 0.0f, 1.0f)) * 0.5f + 0.5f; // ndc to normalized window coords
    mouse_screen *= m_pResolution;                                                                       // normalized window coords to window coords
    glm::ivec2 imouse_screen = glm::ivec2(mouse_screen);

    for (int i = 0; i < points.size(); i++) {
      auto &p = points[i];
      spline.addPoint(p.position);
    }
    lastCanvasMouse = imouse_screen;
    //spline.CalculateDistance();
    SetNextWindowPosition(glm::vec2(30, 200));
    // Text(std::format("Spline length: {:.3f}", spline.Length()));
    if (points.size() == 2 && flags.drawHandles) {

      DrawLine({.position = points[0].position, .color = LIGHT_BLUE}, {.position = points[1].position, .color = LIGHT_BLUE});
    }

    if (points.size() > 3) {

      if (flags.fill) {

        DrawLine({.position = points[0].position, .color = points[0].color.get()},
                 {.position = points[points.size() - 2].position, .color = points[points.size() - 2].color.get()});
      }
      const int subdivisions = 50;

      for (int i = 0; i < points.size() - 3; i += 3) {
        for (int j = 0; j < subdivisions; j++) {
          float t = (float)(j) / (float)subdivisions;
          float t2 = (float)(j + 1) / (float)subdivisions;

          auto &p0 = points[i];
          auto &p1 = points[(i + 3) % points.size()];

          auto sp0 = spline.getPoint(t + i);
          auto sp1 = spline.getPoint(t2 + i - ((subdivisions - 1 == j) ? 1e-5 : 0.0));

          if (flags.fill) {
            fill_vertices.push_back(
                {.pos_uv = glm::vec4(points[0].position.x / m_pResolution.x, points[0].position.y / m_pResolution.y, font->white_pixel.x, font->white_pixel.y),
                 .color = p0.color.get(),
                 .vtx_pos = glm::vec2(-1.0f),
                 .padding = glm::vec2(-1.0f)});
            fill_vertices.push_back({.pos_uv = glm::vec4(sp0.x / m_pResolution.x, sp0.y / m_pResolution.y, font->white_pixel.x, font->white_pixel.y),
                                     .color = p0.color.get(),
                                     .vtx_pos = glm::vec2(-1.0f),
                                     .padding = glm::vec2(-1.0f)});
            fill_vertices.push_back({.pos_uv = glm::vec4(sp1.x / m_pResolution.x, sp1.y / m_pResolution.y, font->white_pixel.x, font->white_pixel.y),
                                     .color = p0.color.get(),
                                     .vtx_pos = glm::vec2(-1.0f),
                                     .padding = glm::vec2(-1.0f)});
          }

          auto g = spline.getGradient(t + i);
          float acc = length(g);
          auto grad = glm::normalize(g);
          auto normal = normalize(rot(glm::radians(90.0f)) * grad);
          if (flags.drawTangents) {
            DrawLine({.position = sp0, .color = glm::vec4(grad, 0.0, 1.0)}, {.position = (sp0 + grad * acc * 0.25), .color = glm::vec4(grad, 1.0, 1.0)});
          }
          if (flags.drawNormals) {
            DrawLine({.position = sp0, .color = glm::vec4(normal, 0.0, 1.0)}, {.position = (sp0 + normal * 80.0), .color = glm::vec4(normal, 1.0, 1.0)});
          }
          DrawLine({.position = sp0, .color = segment_color(p0, p1, t)},
                   {.position = (sp1 + glm::normalize(sp1 - sp0) * 0.0), .color = segment_color(p0, p1, t2)});
        }
      }
      if (flags.drawHandles) {
        for (int i = 0; i < points.size() - 1; i += 3) {
          DrawLine({.position = points[i].position, .color = LIGHT_BLUE}, {.position = points[i + 1].position, .color = LIGHT_BLUE});
          if (i > 0)
            DrawLine({.position = points[i].position, .color = LIGHT_BLUE}, {.position = points[i - 1].position, .color = LIGHT_BLUE});
        }

        DrawLine({.position = points[points.size() - 1].position, .color = LIGHT_BLUE},
                 {.position = points[points.size() - 1 - 1].position, .color = LIGHT_BLUE});
      }
     /* float l = spline.Length();
      const float interval = 50.0f;
      float delta = 0.0f;
      while (delta < l) {
        auto p = spline.getPoint(spline.getT(delta));
        delta += interval;
        m_pCursorPosition = p;
        Circle(20, 5);
      }*/
    }
    if (flags.drawHandles) {
      for (int i = 0; i < points.size(); i++) {
        auto &p = points[i];

        SetNextWindowPosition(p.position + glm::vec2(-10.0, -50.0));
        // Text(num);

        m_pCursorPosition = p.position;
        float color = 1.0;
        int subdivs = 15;
        if (i % 3 > 0)
          subdivs = 4;
        Circle(subdivs, 12, glm::vec4(0.0, 0.0, 0.0, color));
        Circle(subdivs, i == hovered_point ? 8 : 4, p.color.get());
        // Text("({}, {})", p.position.x, p.position.y);
      }
      if (selected_point == -1 && m_pMouse.right_pressed && points.size() >= 1) {
        T handle_position = T(1.0f);
        handle_position.x = imouse_screen.x;
        handle_position.y = imouse_screen.y;
        points.push_back({.position = handle_position, .color = LIGHT_BLUE});
        points.back().internal_position = points.back().position;
        points.push_back({.position = handle_position, .color = WHITE});
        points.back().internal_position = points.back().position;
        points.push_back({.position = handle_position, .color = LIGHT_BLUE});
        points.back().internal_position = points.back().position;
        selected_point = points.size() - 1;

      } else if (points.size() == 0 && m_pMouse.right_pressed) {
        T handle_position = T(1.0f);
        handle_position.x = imouse_screen.x; // m_pMouse.position.x;
        handle_position.y = imouse_screen.y; // m_pMouse.position.y;
        points.push_back({.position = handle_position, .color = WHITE});
        points.back().internal_position = points.back().position;
        points.push_back({.position = handle_position, .color = LIGHT_BLUE});
        points.back().internal_position = points.back().position;
        selected_point = points.size() - 1;
      }
    }

    if (flags.fill) {
      CheckShader();
      CreatePolygon(fill_vertices);
    }
  }
};
} // namespace UI
