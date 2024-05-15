#pragma once

#include <glm/glm.hpp>
#include <queue>
#include <vector>

#include <core/engine/renderer/renderer.hpp>
#include <core/engine/renderer/resource_manager.hpp>
#include <core/framework/IO/Mouse.hpp>

#include <core/framework/common.hpp>
#include <core/framework/utils/file.hpp>
#include <core/framework/utils/image.hpp>

// #define STBTT_STATIC
// #define STB_TRUETYPE_IMPLEMENTATION
// #include "../thirdparty/stb_truetype.h"

#include "Roboto.hpp"

namespace UI
{
  struct Window
  {
    size_t id;
    glm::vec2 size = glm::vec2(0.0);
    glm::vec2 position{};
    size_t vtx_index = 0;
    bool fixed_size = false;
    glm::vec2 setted_size;
  };

  struct Mouse
  {
    glm::ivec2 position;
    bool left_down = false;
    bool left_was_down = false;
    bool right_down = false;
    bool left_pressed = false;
    bool left_released = false;
    bool right_pressed = false;
    bool right_released = false;
  };

  struct AngularButtonWidget
  {
    int subdivisions = 1;
    int buttonCount = 1;
    int currentButtonIndex = 0;
    float angular_step = 3.14159f;
    float innerRadius = 100;
    float outerRadius = 200;
    glm::vec2 position;
  };

  struct Context
  {
    glm::ivec2 window_size{};
    glm::ivec2 cursor_position{};
    glm::ivec2 mouse_position{};

    glm::vec2 prev_widget_size{};
    glm::vec2 prev_widget_position{};

    uint32_t pending_actions = 0;
    std::unordered_map<size_t, Window> windows;
    AngularButtonWidget currentAngularWidget;
  };

  struct UIVertex
  {
    glm::vec4 pos_uv{};
    glm::vec4 color = glm::vec4(1.0, 1.0, 1.0, 0.5);
  };

  struct Rect
  {
    glm::vec2 position;
    glm::vec2 size;
  };

  struct Style
  {

    float windowPadding = 5.0f;
    float buttonPadding = 5.0f;
    float buttonHeight = 20;
    glm::vec2 textSize = glm::vec2(10, 20);
    float fontWidth = 12;
  };

  struct DrawCommand
  {
    int32_t vertex_offset;
    int32_t num_vertices;
    engine::Ref<engine::BindGroup> bind_group;
  };

  enum class DockLocation : uint8_t
  {
    TOP = 0,
    BOTTOM,
    LEFT,
    RIGHT
  };

  struct UI
  {

  private:
    std::vector<DrawCommand> m_DrawLists;
    std::shared_ptr<engine::ResourceManager> m_pResourceManager = nullptr;
    std::shared_ptr<engine::Renderer> m_pRenderer = nullptr;
    Context m_pContext;
    std::vector<engine::Ref<engine::Buffer>> m_pVertexBuffer;
    engine::Ref<engine::Mesh> m_pMesh; // dummy mesh to pass the vertex buffer
    engine::Ref<engine::BindGroup> m_pBindGroup;
    engine::Ref<engine::RenderPass> m_pRenderPass;
    uint32_t pending_actions = 0;
    glm::vec2 m_pCursorPosition = glm::vec2(0.0f);
    float m_pAspectRatio = 1280.0f / 720.0f;
    glm::vec2 m_pResolution = glm::vec2(1280.0f, 720.0f);
    framework::Window *m_pWindow;
    Mouse m_pMouse;
    Style m_pStyle;

  public:
    std::vector<UIVertex> m_pVertices;

    size_t vertices_pushed = 0;
    size_t commands_pushed = 0;

    Window *currentWindow = nullptr;
    // std::unordered_map<std::string, engine::Ref<engine::BindGroup>>

    // stbtt_bakedchar cdata[95]; // ASCII 32..126 is 95 glyphs

    int texture_size = 128;

    struct font_pixel
    {
      uint8_t r, g, b, a;
    };
    std::vector<font_pixel> pixels;
    int highest_character = 0;

    engine::ShaderInfo loadShaders()
    {

      auto vertexCode = utils::readFile("./assets/shaders/ui-vertex.spv");
      auto fragmentCode = utils::readFile("./assets/shaders/ui-fragment.spv");
      engine::ShaderInfo mainShaderInfo = {
          .stages = {{.entryPoint = "main",
                      .shaderCode = vertexCode,
                      .stage = engine::VERTEX},
                     {.entryPoint = "main",
                      .shaderCode = fragmentCode,
                      .stage = engine::FRAGMENT}},
          .usedStages =
              engine::ShaderModuleStage(engine::VERTEX | engine::FRAGMENT),
          .name = "UIShader"};

      return mainShaderInfo;
    }

    std::array<Roboto::AlignedQuad, 95> m_AlignedQuads;

    glm::vec2 m_WhitePixelPos{};
    engine::PipelineLayout uiLayout;
    void Init(std::shared_ptr<engine::ResourceManager> resource_manager,
              std::shared_ptr<engine::Renderer> renderer,
              framework::Window *window)
    {
      m_pResourceManager = resource_manager;
      m_pRenderer = renderer;
      m_pWindow = window;

      m_pVertices.resize(10000);
      pixels.resize(texture_size * texture_size);

      engine::ShaderInfo mainShaderInfo = loadShaders();

      engine::VertexLayout vertexLayout = {
          .descriptors =
              {
                  {engine::XYZW_FLOAT, offsetof(UIVertex, pos_uv)},
                  {engine::XYZW_FLOAT, offsetof(UIVertex, color)},
              },
          .size = sizeof(UIVertex)};

      uiLayout = {
          .shaderInfo = mainShaderInfo,
          .vertexLayout = vertexLayout,
          .cullMode = engine::CullMode::NONE,
          .windingMode = engine::WindingMode::CLOCKWISE,
          .depthWriteEnable = false,
          .depthTestEnable = false,
      };
      /*
                              engine::RenderPassInfo renderPassInfo =
                                      engine::RenderPassFactory()
                                              .name("Default")
                                              .depthAttachment(true)
                                              .isSwapChainAttachment(true)
                                              .subpasses({})
                                              .dimensions({.width = 1280, .height
         = 720}) .inputs({{.size = 384, .offset = 0, .binding = 0, .set = 0, .type
         = engine::UniformBindingType::UNIFORM_BUFFER, .descriptorCount = 1, .name
         = "UIUniformBuffer"},
                                                               {
                                                                       .binding =
         1, .type = engine::UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER,
                                                                       .name =
         "font_image",
                                                               }})
                                              .outputs({{
                                                                        .format =
         engine::COLOR_RGBA, .blendEnable = true,

                                                                        .clearColor
         = {0.1f, 0.1f, 0.1f, 0.0f}, .isDepthAttachment = false,
                                                                        .isSwapChainAttachment
         = true, .clearAttachment = true,
                                                                },
                                                                {.format =
         engine::DEPTH_F32_STENCIL_8, .depthStencilValue = {1, 0},
                                                                 .isDepthAttachment
         = true}}) .pipelineLayout(uiLayout) .pushConstant(72);

                              m_pRenderPass =
         m_pResourceManager->createDefaultRenderPass(renderPassInfo);*/
      // m_pVertices.resize(10000);

      m_pVertexBuffer.resize(m_pResourceManager->FramesInFlight());

      IO::Info("Allocating ", m_pResourceManager->FramesInFlight(),
               " vertex buffers for UI.");

      for (int i = 0; i < m_pResourceManager->FramesInFlight(); i++)
      {

        m_pVertexBuffer[i] = m_pResourceManager->createMappedVertexBuffer(
            "UIVertexBuffer" + std::to_string(i),
            {.size = 10000 * sizeof(UIVertex)});
      }

      // unsigned char* ttf_buffer = new unsigned char[1 << 20];
      // unsigned char* temp_bitmap = new unsigned char[texture_size *
      // texture_size]; fread(ttf_buffer, 1, 1 << 20,
      // fopen("./assets/fonts/Roboto-Regular.ttf", "rb"));
      // stbtt_BakeFontBitmap(ttf_buffer, 0, 18.0, temp_bitmap, texture_size,
      // texture_size, 32, 95, cdata);
      /*
      for (int i = 0; i < 95; i++) {
              char c = i;
              float x = 0, y = 0;
              stbtt_aligned_quad q;
              stbtt_GetBakedQuad(cdata, texture_size, texture_size, c, &x, &y, &q,
      1);

              m_AlignedQuads[i] = { q.x0,q.y0,q.s0,q.t0,q.x1,q.y1,q.s1,q.t1 };
      }

      for (int i = 0; i < texture_size * texture_size; i++)
      {
              pixels[i] = { 255,
                                       255,
                                       255,
                                       temp_bitmap[i] };

              if (temp_bitmap[i] == 255) {
                      m_WhitePixelPos = glm::vec2(i % 256, i / 256) / 255.0f;
              }
      }

      delete[] temp_bitmap;
      delete[] ttf_buffer;

      std::ofstream outHeader("Roboto.hpp");
      outHeader << "//Roboto font\n";
      outHeader << "namespace Roboto {\n";
      outHeader << "struct AlignedQuad\n{\n\tfloat x0, y0, s0, t0; //
      top-left\n\tfloat x1, y1, s1, t1; // bottom-right\n};\n"; outHeader <<
      "static const unsigned char pixels[] = {\n";

      for (int i = 0; i < texture_size * texture_size; i++)
      {
              outHeader << (int)pixels[i].a << ",";
              if (i % 40 == 0 && i != 0) outHeader << "\n";
      }

      outHeader << "};\n\n";
      outHeader << "static const AlignedQuad glyphs[] = {\n";

      for (int i = 0; i < 95; i++) {
              outHeader << "{" << m_AlignedQuads[i].x0 << ", " <<
      m_AlignedQuads[i].y0 << ", " << m_AlignedQuads[i].s0 << ", " <<
      m_AlignedQuads[i].t0 << ","; outHeader << m_AlignedQuads[i].x1 << ", " <<
      m_AlignedQuads[i].y1 << ", " << m_AlignedQuads[i].s1 << ", " <<
      m_AlignedQuads[i].t1 << "},"; if (i % 2 == 0) outHeader << "\n";
      }
      outHeader << "};\n";
      outHeader << "}\n";*/

      for (int i = 0; i < texture_size * texture_size; i++)
      {
        pixels[i] = {255, 255, 255, Roboto::pixels[i]};

        if (Roboto::pixels[i] == 255)
        {
          m_WhitePixelPos = glm::vec2(i % 256, i / 256) / 256.0f;
        }
      }

      for (int i = 0; i < 95; i++)
      {
        m_AlignedQuads[i] = Roboto::glyphs[i];

        int glyph_size = abs(m_AlignedQuads[i].y0 - m_AlignedQuads[i].y1);

        if (glyph_size > highest_character)
        {
          highest_character = glyph_size;
        }
      }

      engine::TextureCreationInfo texInfo;
      texInfo.pixels = (unsigned char *)pixels.data();
      texInfo.width = texture_size;
      texInfo.height = texture_size;
      texInfo.format = engine::TextureFormat::COLOR_RGBA;
      texInfo.generateMipMaps = false;
      texInfo.mipLevels = 1;
      texInfo.size = texture_size * texture_size * 4;

      m_pBindGroup = m_pResourceManager->createBindGroup({
          .bindingInfo = {{
              .binding = 2,
              .type = engine::UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER,
              .textureInfo = texInfo,
              .name = "font_image",
          }},
          .inputs = {},
          .renderPass = "SwapChainPass",
          .name = "UIBindGroup",
      });

      m_pMesh = m_pResourceManager->insertMesh(
          "UIMesh", {.vertexBuffer = m_pVertexBuffer[0], .numVertices = 0});

      m_DrawLists.resize(1);
    }

    std::unordered_map<std::string, engine::Ref<engine::BindGroup>> m_pBindGroups;

    void AddRenderTarget(const char *name, uint32_t rto_index,
                         const std::string &renderPass)
    {

      m_pBindGroups[name] = m_pResourceManager->createBindGroup({
          .bindingInfo = {},
          .inputs = {{.renderPass = renderPass,
                      .index = rto_index,
                      .bindingPoint = 2}},
          .renderPass = "SwapChainPass",
          .name = name,
      });
    }

    void AddImageFromReference(const char *image, const char *name)
    {

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

    void AddImageFromFile(const char *path, const char *name)
    {
      int w, h, c = 4;
      unsigned char *pixels = framework::load_image_from_file(path, &w, &h, &c);

      engine::TextureCreationInfo texInfo;
      texInfo.pixels = (unsigned char *)pixels;
      texInfo.width = w;
      texInfo.height = h;
      texInfo.format = engine::TextureFormat::COLOR_RGBA;
      texInfo.generateMipMaps = false;
      texInfo.mipLevels = 1;
      texInfo.size = texture_size * texture_size * 4;

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

    void Update();
    void Draw(engine::Ref<engine::RenderPass>);
    void Free();

    glm::vec2 CursorPosition();

    glm::vec2 ToScreenCoords(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &proj, const glm::vec2 &resolution);

    void CreateRect(glm::vec2 position, glm::vec2 size, glm::vec2 uv0,
                    glm::vec2 uv1,
                    glm::vec4 color = glm::vec4(0.11, 0.11, 0.11, 0.8));

    void CreatePolygon(std::vector<UIVertex> vertices);

    void SetNextWindowPosition(glm::vec2 position);

    void BeginWindow(const std::string &name, glm::vec2 size = glm::vec2(0.0f));

    void EndWindow();

    void Text(const std::string &text, glm::vec4 color = glm::vec4(1.0f),
              bool standalone = true);
    void Text(const std::string &text, glm::vec2 position,
              glm::vec4 color = glm::vec4(1.0f), bool standalone = true);

    bool Button(const std::string &text);
    void Sameline();

    void Dock(DockLocation, float thickness);

    bool ImageButton(const std::string &, const std::string &texture,
                     glm::vec2 size, glm::vec2 uv0, glm::vec2 uv1,
                     glm::vec4 tint = glm::vec4(1.0f));

    void AngularButtons(glm::vec2 position, float innerRadius, float outerRadius,
                        int count);
    bool AngularButton(const std::string &text);

    bool CheckBox(const std::string &text, bool *state);

    void HealthBar(const std::string &text, glm::vec2 position, float val,
                   float min, float max);

    void Scale(const std::string &text, int &current, int min, int max,
               glm::vec2 size);
  };
} // namespace UI