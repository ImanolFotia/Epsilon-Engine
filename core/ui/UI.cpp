
#include "UI.hpp"

namespace UI
{
  void UI::CheckShader()
  {

    if (m_DrawLists[commands_pushed].bind_group != m_CurrentBindGroup)
    {
      auto &new_command = m_DrawLists.emplace_back();
      commands_pushed++;
      new_command.vertex_offset = vertices_pushed;
      new_command.num_vertices = 0;
      new_command.bind_group = m_CurrentBindGroup;
    }
  }

  glm::vec2 UI::textSize(const std::string &text)
  {
    auto font = m_Fonts[current_font];
    float ox = 0, oy = 0;
    char *text_data;
    char *text_data_orig;

    text_data = new char[text.size() + 1];
    text_data_orig = text_data;

    std::memcpy(text_data, text.c_str(), text.size() + 1);
    while (*text_data)
    {
      if (*text_data >= font->first_char && *text_data < font->first_char + font->num_chars)
      {
        float x = 0, y = 0;
        // stbtt_aligned_quad q;
        // stbtt_GetBakedQuad(cdata, texture_size, texture_size, *text_data - 32,
        // &x, &y, &q, 1); // 1=opengl & d3d10+,0=d3d9
        AlignedQuad q = m_Fonts[DEFAULT_FONT]->Glyphs()[(int)(*text_data) - font->first_char];

        if (*text_data == ' ')
        {
          ox += 5;
        }
        else
        {
          ox += q.x1;
          oy = glm::max(oy, q.y1 - q.y0);
        }
      }

      ++text_data;
    }

    delete[] text_data_orig;

    return {ox, oy};
  }

  void UI::Update()
  {

    m_pCursorPosition = glm::vec2(0.0f);
    m_pResolution =
        glm::vec2(m_pWindow->getSize().width, m_pWindow->getSize().height);
    m_pAspectRatio = m_pResolution.x / m_pResolution.y;

    glm::vec2 mouse =
        glm::vec2(framework::Input::Mouse::XPOS, framework::Input::Mouse::YPOS);
    m_pMouse.position = mouse;

    m_pMouse.left_down =
        framework::Input::Mouse::LEFT == framework::Input::STATE::PRESSED;
    m_pMouse.right_down =
        framework::Input::Mouse::RIGHT == framework::Input::STATE::PRESSED;

    if (m_pMouse.left_down && m_pMouse.left_pressed == false &&
        m_pMouse.left_was_down == false)
    {
      m_pMouse.left_pressed = true;
    }
    else
    {
      m_pMouse.left_pressed = false;
    }

    if (m_pMouse.right_down && !m_pMouse.right_pressed)
    {
      m_pMouse.right_pressed = true;
    }
    else
    {
      m_pMouse.right_pressed = false;
    }

    m_pMouse.left_was_down = m_pMouse.left_down;
  }
  void UI::Draw(engine::Ref<engine::RenderPass> renderPass)
  {

    m_pResourceManager->UpdateMappedBuffer(
        m_pVertexBuffer[m_pRenderer->CurrentFrameInFlight()],
        {.size = vertices_pushed * sizeof(UIVertex),
         .offset = 0,
         .vertices = (void *)m_pVertices.data()});
    m_pResourceManager->UpdateMesh(
        m_pMesh,
        {
            .vertexBuffer = m_pVertexBuffer[m_pRenderer->CurrentFrameInFlight()],
        });
    if (m_pResolution.x > 0)
    {
      // m_pRenderer->BeginFrame();
      // m_pRenderer->Begin();
      // m_pRenderer->SetRenderPass(m_pRenderPass);
      for (int i = 0; i < commands_pushed + 1; i++)
      {
        auto &draw_command = m_DrawLists[i];
        m_pResourceManager->UpdateMesh(
            m_pMesh, {.vertex_size = draw_command.num_vertices,
                      .vertex_offset = draw_command.vertex_offset});

        m_pRenderer->Push(
            {.mesh = m_pMesh,
             .material = draw_command.bind_group,
             .pushConstant = engine::Ref<engine::PushConstant>::makeEmpty(),
             .objectConstant = {.transform = glm::mat4(1.0f),
                                .material_index = 0,
                                .animation_offset = 0},
             .layout_index = 1,
             .uniformIndex = 0});

        draw_command.bind_group = m_CurrentBindGroup;
        draw_command.num_vertices = 0;
        draw_command.vertex_offset = 0;
      }

      m_pRenderer->Flush(renderPass, engine::DrawType::NON_INDEXED);
      glm::vec3 v = glm::vec3(1.0f);
      // m_pRenderer->End(v);

      // m_pRenderer->Submit();
      // m_pRenderer->EndFrame();
    }
    m_DrawLists.resize(1);
    commands_pushed = 0;
    vertices_pushed = 0;
  }
  void UI::Free() {}

  void UI::CreateRect(glm::vec2 position, glm::vec2 size,
                      glm::vec2 uv0 = glm::vec2(0.0f),
                      glm::vec2 uv1 = glm::vec2(1.0f), glm::vec4 color)
  {
    size /= m_pResolution;
    position /= m_pResolution;
    m_pVertices[vertices_pushed].color = color;
    m_pVertices[vertices_pushed++].pos_uv =
        glm::vec4(position.x, position.y, uv0.x, uv0.y);

    m_pVertices[vertices_pushed].color = color;
    m_pVertices[vertices_pushed++].pos_uv =
        glm::vec4(position + glm::vec2(size.x, 0.0f), uv1.x, uv0.y);

    m_pVertices[vertices_pushed].color = color;
    m_pVertices[vertices_pushed++].pos_uv =
        glm::vec4(position + glm::vec2(0.0f, size.y), uv0.x, uv1.y);

    m_pVertices[vertices_pushed].color = color;
    m_pVertices[vertices_pushed++].pos_uv =
        glm::vec4(position + glm::vec2(0.0f, size.y), uv0.x, uv1.y);

    m_pVertices[vertices_pushed].color = color;
    m_pVertices[vertices_pushed++].pos_uv =
        glm::vec4(position + glm::vec2(size.x, 0.0f), uv1.x, uv0.y);

    m_pVertices[vertices_pushed].color = color;
    m_pVertices[vertices_pushed++].pos_uv =
        glm::vec4(position + size, uv1.x, uv1.y);

    auto &current_draw_command = m_DrawLists[commands_pushed];
    current_draw_command.num_vertices =
        vertices_pushed - current_draw_command.vertex_offset;
  }

  void UI::CreatePolygon(std::vector<UIVertex> vertices)
  {
    for (int i = 0; i < vertices.size(); i++)
    {
      m_pVertices[vertices_pushed++] = vertices[i];
    }

    auto &current_draw_command = m_DrawLists[commands_pushed];
    current_draw_command.num_vertices =
        vertices_pushed - current_draw_command.vertex_offset;
  }

  void UI::SetNextWindowPosition(glm::vec2 position)
  {
    m_pCursorPosition = position;
  }

  void UI::Spacer(glm::vec2 size) { m_pCursorPosition += size; }

  void UI::BeginWindow(const std::string &name, glm::vec2 size, glm::vec4 color)
  {
    CheckShader();
    size_t id = std::hash<std::string>{}(name);
    if (!m_pContext.windows.contains(id))
    {
      m_pContext.windows[id] = {id};
    }

    currentWindow = &m_pContext.windows[id];
    currentWindow->fixed_size = glm::dot(size, size) > 0.0 ? false : true;
    currentWindow->size = glm::vec2(0.0);
    currentWindow->vtx_index = vertices_pushed;
    currentWindow->setted_size = size;
    currentWindow->color = color;
    // m_pCursorPosition.y += m_pStyle.windowPadding;
    currentWindow->position = m_pCursorPosition;

    vertices_pushed += 6;
    auto &current_draw_command = m_DrawLists[commands_pushed];
    current_draw_command.num_vertices =
        vertices_pushed - current_draw_command.vertex_offset;

    pending_actions++;
  }

  void UI::EndWindow()
  {
    assert(currentWindow != nullptr);
    glm::vec2 size = currentWindow->fixed_size ? currentWindow->size
                                               : currentWindow->setted_size;

    m_pContext.prev_widget_position = m_pCursorPosition;

    size_t temp_vtx = vertices_pushed;
    vertices_pushed = currentWindow->vtx_index;

    glm::vec2 white_pixel = glm::vec2(m_Fonts[current_font]->white_pixel.x, m_Fonts[current_font]->white_pixel.y);

    CreateRect(currentWindow->position,
               size + glm::vec2(m_pStyle.windowPadding * 3.0f,
                                m_pStyle.windowPadding * 3.0f),
               white_pixel, white_pixel, currentWindow->color);

    vertices_pushed = temp_vtx;

    auto &current_draw_command = m_DrawLists[commands_pushed];
    current_draw_command.num_vertices =
        vertices_pushed - current_draw_command.vertex_offset;

    m_pCursorPosition.y += m_pStyle.windowPadding * 2;

    m_pContext.prev_widget_size = size + (m_pStyle.windowPadding * 2);

    currentWindow = nullptr;
    pending_actions--;
  }

  void UI::Text(const std::string &text, glm::vec4 color, bool standalone)
  {
    glm::vec2 text_size = textSize(text);
    // m_pCursorPosition += 5.0f;
    Text(text, m_pCursorPosition, color, standalone);
    m_pCursorPosition.y += text_size.y + 5.0f;
    // m_pCursorPosition.x -= 5.0f;
  }

  void UI::Text(const std::string &text, glm::vec2 position, glm::vec4 color,
                bool standalone)
  {
    auto font = m_Fonts[current_font];
    CheckShader();

    m_pContext.prev_widget_position = position;
    glm::vec2 text_size = textSize(text);

    glm::vec2 characters_size{};

    char *text_data;

    char *text_data_orig;
    text_data = new char[text.size() + 1];
    text_data_orig = text_data;

    std::memcpy(text_data, text.c_str(), text.size() + 1);

    if (standalone)
      position.x += 5.0f;

    while (*text_data)
    {
      if (*text_data >= font->first_char && *text_data < font->first_char + font->num_chars)
      {
        float x = 0, y = 0;
        std::vector<UIVertex> vertices;
        vertices.resize(6);

        // stbtt_aligned_quad q;
        // stbtt_GetBakedQuad(cdata, texture_size, texture_size, *text_data - 32,
        // &x, &y, &q, 1); // 1=opengl & d3d10+,0=d3d9

        AlignedQuad q = font->Glyphs()[(int)(*text_data) -  font->first_char];

        if (*text_data == ' ')
        {
          position.x += 5;
        }
        else
        {

          glm::vec4 a = glm::vec4((position) / m_pResolution +
                                      glm::vec2(q.x0, q.y0 + font->highest_character) /
                                          m_pResolution,
                                  q.s0, q.t0);
          glm::vec4 b = glm::vec4((position) / m_pResolution +
                                      glm::vec2(q.x1, q.y0 + font->highest_character) /
                                          m_pResolution,
                                  q.s1, q.t0);
          glm::vec4 c = glm::vec4((position) / m_pResolution +
                                      glm::vec2(q.x0, q.y1 + font->highest_character) /
                                          m_pResolution,
                                  q.s0, q.t1);
          glm::vec4 d = glm::vec4((position) / m_pResolution +
                                      glm::vec2(q.x1, q.y1 + font->highest_character) /
                                          m_pResolution,
                                  q.s1, q.t1);

          vertices[0] = {a, color};
          vertices[1] = {b, color};
          vertices[2] = {c, color};

          vertices[3] = {c, color};
          vertices[4] = {b, color};
          vertices[5] = {d, color};

          CreatePolygon(vertices);

          position.x += q.x1;
        }

        characters_size.x = position.x;
        characters_size.y = glm::max(q.y1 + text_size.y, characters_size.y);
      }

      ++text_data;
    }

    if (currentWindow && standalone)
    {
      currentWindow->size.x =
          glm::max(characters_size.x + 2.0f, currentWindow->size.x);
      currentWindow->size.y += characters_size.y + 5;

      m_pContext.prev_widget_size.x =
          characters_size.x - m_pContext.prev_widget_position.x;
      m_pContext.prev_widget_size.y =
          glm::max(characters_size.y + 5.0f, currentWindow->size.y);
    }

    delete[] text_data_orig;
  }

  glm::vec2 UI::CursorPosition() { return m_pCursorPosition; }

  glm::vec2 UI::buttonSize(const std::string &text)
  {
    glm::vec2 size = glm::vec2(m_pStyle.buttonPadding,
                               m_pStyle.buttonHeight + m_pStyle.buttonPadding);
    glm::vec2 text_size = textSize(text);
    size += m_pStyle.buttonPadding;
    size.x += text_size.x;
    size += m_pStyle.buttonPadding;
    size.y = 25 + (m_pStyle.buttonPadding * 2.0f);

    return size;
  }

  bool UI::Button(const std::string &text, ButtonFlags flags)
  {
    CheckShader();
    auto font = m_Fonts[current_font];
    glm::vec2 initial_position = m_pCursorPosition;
    const float padding = 5;
    glm::vec2 size = glm::vec2(m_pStyle.buttonPadding,
                               m_pStyle.buttonHeight + m_pStyle.buttonPadding);
    // glm::vec2 text_size = m_pStyle.textSize; // glm::vec2(10, 20);
    glm::vec4 button_color = (flags.is_tab && flags.is_selected_tab) ? glm::vec4(0.5, 0.5, 0.5, 1.0) : glm::vec4(0.2, 0.2, 0.2, 1.0);

    glm::vec2 text_size = textSize(text);

    bool result = false;

    if (flags.fill_width)
    {
      size.x = currentWindow->setted_size.x + m_pStyle.buttonPadding;
    }
    else
    {
      size.x += text_size.x;

      size.x += m_pStyle.buttonPadding;
    }
    size.y += m_pStyle.buttonPadding;
    size.y = font->highest_character + (m_pStyle.buttonPadding * 2.0f);
    m_pCursorPosition += m_pStyle.buttonPadding;
    glm::vec2 position = m_pCursorPosition;

    glm::vec2 mouse =
        glm::vec2(framework::Input::Mouse::XPOS, framework::Input::Mouse::YPOS);
    if (mouse.x > position.x && mouse.x < position.x + size.x &&
        mouse.y > position.y && mouse.y < position.y + size.y)
    {
      button_color = glm::normalize(glm::vec4(222.0f, 134.0f, 2.0f, 255.0f));
    }

    if (mouse.x > position.x && mouse.x < position.x + size.x &&
        mouse.y > position.y && mouse.y < position.y + size.y &&
        m_pMouse.left_pressed)
    {
      button_color = glm::normalize(glm::vec4(130.0f, 50.0f, 2.0f, 255.0f));
      result = true;
    }

    glm::vec2 white_pixel = glm::vec2(font->white_pixel.x, font->white_pixel.y);

    CreateRect(m_pCursorPosition, size, white_pixel, white_pixel,
               button_color);

    glm::vec2 cursor_pos = m_pCursorPosition;
    cursor_pos.x += padding;
    cursor_pos.y += glm::ceil(padding * 0.5);

    if (flags.center_text && flags.fill_width)
    {
      float x_pos = size.x * 0.5f;
      // align right: x_pos = x_pos - text_size.x;
      x_pos = (cursor_pos.x + x_pos) - (text_size.x * 0.5f);
      x_pos = glm::ceil(((int)x_pos % 2 == 0 ? x_pos + 1 : x_pos));
      Text(text, glm::vec2(x_pos, cursor_pos.y), glm::vec4(1.0f), false);
    }
    else
    {
      cursor_pos.x += (int)cursor_pos.x % 2 ? 0 : 1;
      Text(text, cursor_pos, glm::vec4(1.0f), false);
    }
    m_pCursorPosition.x -= m_pStyle.buttonPadding;

    m_pCursorPosition.y += size.y;

    currentWindow->size.x =
        glm::max(m_pStyle.buttonPadding + size.x, currentWindow->size.x);
    currentWindow->size.y += (size.y + m_pStyle.buttonPadding);

    m_pContext.prev_widget_size.x = m_pStyle.buttonPadding + size.x;
    m_pContext.prev_widget_size.y = size.y + m_pStyle.buttonPadding;

    m_pContext.prev_widget_position = initial_position;
    m_pCursorPosition.x = currentWindow->position.x;
    // m_pContext.prev_widget_size.y = glm::abs(m_pCursorPosition.y -
    // m_pContext.prev_widget_position.y);

    return result;
  }

  void UI::Sameline()
  {
    m_pCursorPosition.x =
        m_pContext.prev_widget_position.x + m_pContext.prev_widget_size.x;
    m_pCursorPosition.y =
        m_pContext.prev_widget_position.y; // m_pContext.prev_widget_size.y;
    if (currentWindow)
      currentWindow->size.y -= m_pContext.prev_widget_size.y;
  }

  void UI::Dock(DockLocation location, float thickness)
  {
    CheckShader();
    m_pContext.prev_widget_position = m_pCursorPosition;

    glm::vec2 white_pixel = glm::vec2(m_Fonts[current_font]->white_pixel.x, m_Fonts[current_font]->white_pixel.y);

    switch (location)
    {
    case DockLocation::TOP:
      CreateRect(glm::vec2(0.0f), glm::vec2(m_pResolution.x, thickness),
                 white_pixel, white_pixel);
      m_pCursorPosition = glm::vec2(0.0f, thickness);
      break;
    case DockLocation::BOTTOM:
      CreateRect(glm::vec2(0.0f, m_pResolution.y - thickness),
                 glm::vec2(m_pResolution.x, thickness), white_pixel,
                 white_pixel);
      break;
    case DockLocation::LEFT:
      CreateRect(glm::vec2(0.0f), glm::vec2(thickness, m_pResolution.y),
                 white_pixel, white_pixel);
      m_pCursorPosition = glm::vec2(thickness, 0.0f);
      break;
    case DockLocation::RIGHT:
      CreateRect(glm::vec2(m_pResolution.x - thickness, 0.0f),
                 glm::vec2(thickness, m_pResolution.y), white_pixel,
                 white_pixel);
      break;
    }
  }

  bool UI::ImageButton(const std::string &text, const std::string &texture,
                       glm::vec2 size, glm::vec2 uv0, glm::vec2 uv1,
                       glm::vec4 tint, bool fill_background)
  {
    if (m_DrawLists[commands_pushed].bind_group != m_pBindGroups[texture])
    {
      auto &new_command = m_DrawLists.emplace_back();
      commands_pushed++;
      new_command.vertex_offset = vertices_pushed;
      new_command.num_vertices = 0;
      new_command.bind_group = m_pBindGroups[texture];
    }
    const float padding = 5;
    // glm::vec2 text_size = m_pStyle.textSize; // glm::vec2(10, 20);
    glm::vec4 button_color = tint;

    m_pCursorPosition += m_pStyle.buttonPadding;
    glm::vec2 position = m_pCursorPosition;

    glm::vec2 mouse =
        glm::vec2(framework::Input::Mouse::XPOS, framework::Input::Mouse::YPOS);
    if (mouse.x > position.x &&
        mouse.x < position.x + size.x + m_pStyle.buttonPadding * 2.0f &&
        mouse.y > position.y &&
        mouse.y < position.y + size.y + m_pStyle.buttonPadding * 2.0f)
    {
      button_color = glm::vec4(0.2, 0.5, 0.5, 1.0);
    }

    glm::vec2 image_position =
        m_pCursorPosition + glm::vec2(m_pStyle.buttonPadding);
    glm::vec2 image_size = size;

    glm::vec2 cursor_pos = m_pCursorPosition;
    cursor_pos += padding;

    m_pCursorPosition.x -= m_pStyle.buttonPadding;

    m_pCursorPosition.y += size.y + m_pStyle.buttonPadding;

    float old_y = m_pCursorPosition.y;

    m_pContext.prev_widget_size.x = size.x + m_pStyle.buttonPadding;
    m_pContext.prev_widget_size.y = (size.y + m_pStyle.buttonPadding * 2);

    glm::vec2 text_position =
        glm::vec2(cursor_pos.x + size.x + m_pStyle.buttonPadding,
                  cursor_pos.y - size.y * 0.25);

    // Sameline();
    auto text_size = textSize(text);
    text_position =
        text_position +
        glm::vec2(0.0f, glm::ceil(((int)text_size.y % 2 == 0 ? text_size.y + 1
                                                             : text_size.y) *
                                  0.75));

    m_pCursorPosition.x -=
        m_pStyle.buttonPadding + text_size.x + m_pStyle.buttonPadding;

    m_pCursorPosition.y +=
        old_y - m_pStyle.buttonPadding; // size.y + glm::ceil(text_size.y * 0.75);

    // currentWindow->size.x = glm::max(m_pStyle.buttonPadding * (text.size() > 0
    // ? 3.0f : 2.0f) + size.x, currentWindow->size.x); currentWindow->size.y +=
    // (size.y + m_pStyle.buttonPadding * 2);

    size.x +=
        text_size.x + m_pStyle.buttonPadding * (text.size() > 0 ? 3.0f : 2.0f);
    size.y += m_pStyle.buttonPadding * 2.0f;

    if (fill_background && m_WhitePixelPositions.contains(texture))
      CreateRect(position, size, m_WhitePixelPositions[texture],
                 m_WhitePixelPositions[texture], button_color);

    CreateRect(image_position, image_size, uv0, uv1, glm::vec4(1.0));

    if (text.size() > 0)
      Text(text, text_position, glm::vec4(1.0f));

    m_pContext.prev_widget_size = size;

    m_pContext.prev_widget_position =
        position + glm::vec2(0.0f, -m_pStyle.buttonPadding);

    currentWindow->size.x =
        glm::max((position.x + size.x) - currentWindow->position.x -
                     m_pStyle.buttonPadding * 2.0f,
                 currentWindow->size.x);
    currentWindow->size.y =
        glm::max((position.y + size.y) - currentWindow->position.y -
                     m_pStyle.buttonPadding * 2.0f,
                 currentWindow->size.y);

    m_pCursorPosition.x =
        currentWindow->position.x; // position.x - m_pStyle.buttonPadding;
    m_pCursorPosition.y = position.y + size.y;
    if (mouse.x > position.x && mouse.x < position.x + size.x &&
        mouse.y > position.y && mouse.y < position.y + size.y &&
        m_pMouse.left_pressed)
    {
      return true;
    }
    return false;
  }

  void UI::AngularButtons(glm::vec2 position, float innerRadius,
                          float outerRadius, int count)
  {

    CheckShader();
    m_pContext.prev_widget_position = m_pCursorPosition;
    glm::vec2 center = position;
    int divisions = count;

    float angle_step = (glm::pi<float>() * 2.0) / divisions;

    float min_sub = 4;
    float max_sub = 8;
    int subdivisions = (float)glm::mix(
        min_sub, max_sub, glm::clamp((float)count / 4.0f, 0.0f, 1.0f));

    AngularButtonWidget angularWidget;
    angularWidget.angular_step = angle_step;
    angularWidget.innerRadius = innerRadius;
    angularWidget.outerRadius = outerRadius;
    angularWidget.buttonCount = count;
    angularWidget.subdivisions = subdivisions;
    angularWidget.position = center;

    m_pContext.currentAngularWidget = angularWidget;
  }

  bool UI::AngularButton(const std::string &text)
  {

    CheckShader();

    glm::vec2 text_size = textSize(text);

    auto rotate = [](float angle)
    {
      return glm::vec2(glm::sin(angle), glm::cos(angle));
    };

    AngularButtonWidget &angularWidget = m_pContext.currentAngularWidget;
    float angle_step = angularWidget.angular_step;
    int subdivisions = angularWidget.subdivisions;
    glm::vec2 center = angularWidget.position;
    float innerRadius = angularWidget.innerRadius;
    float outerRadius = angularWidget.outerRadius;

    float mouse_distance = glm::distance((glm::vec2)m_pMouse.position, center);
    glm::vec4 vertex_color_normal = glm::vec4(1.0, 1.0, 1.0, 1.0);
    glm::vec4 vertex_hover_color = glm::vec4(0.0, 1.0, 1.0, 1.0);
    glm::vec4 vertex_color = vertex_color_normal;

    glm::vec2 mouse_2_center = ((glm::vec2)m_pMouse.position) - center;
    // mouse_2_center.y = -mouse_2_center.y;
    mouse_2_center /= m_pResolution;
    mouse_2_center *= 2.0;
    // mouse_2_center.y = -1.0 * mouse_2_center.y;
    mouse_2_center.y *= -1.0f;
    mouse_2_center.x *= m_pResolution.x / m_pResolution.y;

    // mouse_2_center = glm::normalize(mouse_2_center);

    float mouseAngle = std::atan2(mouse_2_center.y, mouse_2_center.x);
    mouseAngle += glm::pi<float>() + (glm::pi<float>() * 1.5f);
    mouseAngle = std::fmod(mouseAngle, glm::pi<float>() * 2.0f);

    bool isHover = false;

    float ratio =
        1.0 - (((outerRadius - innerRadius) * 0.5) / (outerRadius - innerRadius));

    int i = angularWidget.currentButtonIndex;

    glm::vec2 polygon_center{};
    {
      float begin = (angle_step * i);
      float end = (angle_step * (i + 1));

      float begin_near = begin + 0.05f;
      float begin_far = begin + 0.041f;
      float end_near = end - 0.05f;
      float end_far = end - 0.041f;

      vertex_color = vertex_color_normal;

      if (mouse_distance > innerRadius && mouse_distance < outerRadius)
      {
        if (mouseAngle > begin && mouseAngle < end)
        {
          vertex_color = vertex_hover_color;
          isHover = true;
        }
      }

      std::vector<UIVertex> vertices;

      vertices.resize(6 * subdivisions);

      /*
      glm::vec2 a_b = ((center + rotate(begin) * outerRadius) - (center +
      rotate(begin) * innerRadius)); glm::vec2 a_c = ((center + rotate(begin) *
      outerRadius) - (center + rotate(end) * innerRadius)); glm::vec2 b_d =
      ((center + rotate(begin) * innerRadius) - (center + rotate(end) *
      outerRadius)); glm::vec2 c_d = ((center + rotate(end) * innerRadius) -
      (center + rotate(end) * outerRadius));

      float l_a_b = glm::length(a_b);
      float l_a_c = glm::length(a_c);
      float l_b_d = glm::length(b_d);
      float l_c_d = glm::length(c_d);

      float max_length = glm::max(l_a_b, glm::max(l_a_c, glm::max(l_b_d, l_c_d)));

      polygon_center = a_b * 0.5f * (l_a_b / max_length) + a_c * 0.5f * (l_a_c /
      max_length) + b_d * 0.5f * (l_b_d / max_length) + c_d * 0.5f * (l_c_d /
      max_length);
      */

      glm::vec2 white_pixel = glm::vec2(m_Fonts[current_font]->white_pixel.x, m_Fonts[current_font]->white_pixel.y);

      int step = 0;
      for (int j = 0; j < subdivisions * 6; j += 6)
      {
        // float gradient = ((float)j / (((float)subdivisions * 6)-1.0f)) * 2.0f
        // - 1.0f; vertex_color = vertex_color_normal *
        // glm::abs(glm::vec4(gradient, gradient, gradient, 1.0f));

        float v_step_near = (end_near - begin_near);
        float v_step_far = (end_far - begin_far);
        float sub_step_near = (v_step_near / subdivisions);
        float sub_step_far = (v_step_far / subdivisions);

        glm::vec2 subdivision_range_a_near =
            rotate(begin_near + sub_step_near * step);
        glm::vec2 subdivision_range_b_near =
            rotate(begin_near + sub_step_near * (step + 1));

        glm::vec2 subdivision_range_a_far =
            rotate(begin_far + sub_step_far * step);
        glm::vec2 subdivision_range_b_far =
            rotate(begin_far + sub_step_far * (step + 1));

        glm::vec2 a = (center + subdivision_range_a_far * outerRadius);
        glm::vec2 b = (center + subdivision_range_b_near * innerRadius);
        glm::vec2 c = (center + subdivision_range_a_near * innerRadius);
        glm::vec2 d = (center + subdivision_range_b_far * outerRadius);

        /*
         if (mouse_distance > innerRadius && mouse_distance < outerRadius)
    {
        if (mouseAngle > begin && mouseAngle < end)
        {
            vertex_color = vertex_hover_color;
            isHover = true;
        }
    }
    */

        if (isHover)
        {
          float rad_dist = outerRadius - innerRadius;
          float angle_dist = end - begin;
          a += glm::normalize(a - glm::vec2(m_pMouse.position)) * 10.0f;
          b += glm::normalize(b - glm::vec2(m_pMouse.position)) * 10.f;
          c += glm::normalize(c - glm::vec2(m_pMouse.position)) * 10.f;
          d += glm::normalize(d - glm::vec2(m_pMouse.position)) * 10.f;
        }

        vertices[j].pos_uv =
            glm::vec4(a / m_pResolution, white_pixel.x, white_pixel.y);
        vertices[j].color = vertex_color * glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
        vertices[j + 1].pos_uv =
            glm::vec4(b / m_pResolution, white_pixel.x, white_pixel.y);
        vertices[j + 1].color = vertex_color;
        vertices[j + 2].pos_uv =
            glm::vec4(c / m_pResolution, white_pixel.x, white_pixel.y);
        vertices[j + 2].color = vertex_color;
        vertices[j + 3].pos_uv =
            glm::vec4(a / m_pResolution, white_pixel.x, white_pixel.y);
        vertices[j + 3].color =
            vertex_color * glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
        vertices[j + 4].pos_uv =
            glm::vec4(d / m_pResolution, white_pixel.x, white_pixel.y);
        vertices[j + 4].color =
            vertex_color * glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
        vertices[j + 5].pos_uv =
            glm::vec4(b / m_pResolution, white_pixel.x, white_pixel.y);
        vertices[j + 5].color = vertex_color;
        step++;
      }

      for (auto &vtx : vertices)
      {
        polygon_center += (glm::vec2(vtx.pos_uv.x, vtx.pos_uv.y) * m_pResolution);
      }

      polygon_center /= vertices.size();

      CreatePolygon(vertices);

      Text(text,
           polygon_center +
               ((glm::normalize(polygon_center - center) * 2.0f - 1.0f) -
                (text_size * 0.5f)),
           glm::vec4(1.0f), false);
    }

    angularWidget.currentButtonIndex++;
    if (isHover && m_pMouse.left_pressed)
    {
      return true;
    }

    return false;
  }

  bool UI::CheckBox(const std::string &text, bool *state)
  {
    CheckShader();
    auto font = m_Fonts[current_font];

    m_pContext.prev_widget_position = m_pCursorPosition;
    const float switch_width = 20;
    const float padding = 5;
    glm::vec2 size = glm::vec2(m_pStyle.buttonPadding,
                               m_pStyle.buttonHeight + m_pStyle.buttonPadding);
    // glm::vec2 text_size = m_pStyle.textSize; // glm::vec2(10, 20);
    glm::vec4 button_color = glm::vec4(0.2, 0.3, 0.8, 1.0);

    glm::vec2 text_size = textSize(text);

    size.x += text_size.x;

    size += m_pStyle.buttonPadding;
    size.x += switch_width;
    size.y = text_size.y + (m_pStyle.buttonPadding * 2.0f);
    m_pCursorPosition += m_pStyle.buttonPadding;
    glm::vec2 position = m_pCursorPosition;

    if (*state)
    {
      button_color = glm::vec4(0.0, 0.7, 0.0, 1.0);
    }
    else
    {
      button_color = glm::vec4(0.7, 0.0, 0.0, 1.0);
    }

    glm::vec2 mouse =
        glm::vec2(framework::Input::Mouse::XPOS, framework::Input::Mouse::YPOS);
    if (mouse.x > position.x && mouse.x < position.x + switch_width &&
        mouse.y > position.y && mouse.y < position.y + switch_width)
    {
      // button_color = glm::vec4(0.2, 0.5, 0.5, 1.0);
    }

    glm::vec2 white_pixel = glm::vec2(m_Fonts[current_font]->white_pixel.x, m_Fonts[current_font]->white_pixel.y);

    glm::vec2 cursor_pos = m_pCursorPosition;
    cursor_pos += padding;

    glm::vec2 text_padding = glm::vec2(m_pStyle.buttonPadding, 0.0f);

    CreateRect(position, glm::vec2(switch_width), white_pixel,
               white_pixel, glm::vec4(0.0, 0.0, 0.0, 1.0));
    CreateRect(position + glm::vec2(switch_width * 0.1),
               glm::vec2(switch_width) * 0.8f, white_pixel, white_pixel,
               button_color);

    int glyph_size = abs(m_Fonts[DEFAULT_FONT]->Glyphs()[(int)'x' - 32].y0 -
                         m_Fonts[DEFAULT_FONT]->Glyphs()[(int)'x' - 32].y1);
    // std::string tick = '√';
    if (*state)
      Text("x",
           position +
               glm::vec2(switch_width * 0.3f, -glm::ceil(glyph_size * 0.25)),
           glm::vec4(1.0f), false);

    Text(text,
         cursor_pos +
             glm::vec2(switch_width, -glm::ceil(font->highest_character * 0.25)),
         glm::vec4(1.0f), false);

    m_pCursorPosition.x -= m_pStyle.buttonPadding;

    m_pCursorPosition.y += size.y;

    currentWindow->size.x =
        glm::max(m_pStyle.buttonPadding + size.x, currentWindow->size.x);
    currentWindow->size.y += (size.y + m_pStyle.buttonPadding);

    m_pContext.prev_widget_size.x =
        switch_width + text_size.x + m_pStyle.buttonPadding;
    m_pContext.prev_widget_size.y = (size.y + m_pStyle.buttonPadding);

    if (mouse.x > position.x && mouse.x < position.x + switch_width &&
        mouse.y > position.y && mouse.y < position.y + switch_width &&
        m_pMouse.left_pressed)
    {
      *state = !*state;
      return true;
    }
    return false;
  }

  void UI::HealthBar(const std::string &text, glm::vec2 position, float val,
                     float min, float max)
  {

    CheckShader();
    m_pContext.prev_widget_position = m_pCursorPosition;
    const float padding = 1;
    glm::vec2 size = glm::vec2(m_pStyle.buttonPadding,
                               m_pStyle.buttonHeight + m_pStyle.buttonPadding);
    glm::vec4 button_color = glm::vec4(0.5, 0.5, 0.5, 1.0);

    glm::vec2 text_size = textSize(text);

    size = glm::vec2(100, 10);

    const int num_squares = glm::min(10, (int)((val / max) * 10) + 1);

    glm::vec2 white_pixel = glm::vec2(m_Fonts[current_font]->white_pixel.x, m_Fonts[current_font]->white_pixel.y);

    CreateRect(position, size, white_pixel, white_pixel, button_color);

    glm::vec2 sqr_position = position + padding;
    glm::vec2 sqr_size = glm::vec2((size.x - (padding * (10 + 1))) / 10,
                                   size.y - (padding * 2.0f));
    float last_sqr_width = 10.0f - ((max / 10.0f) - (fmod(val, 10.0f)));
    int i = 0;
    glm::vec4 color = glm::mix(glm::vec4(1.0, 0.0, 0.0, 1.0),
                               glm::vec4(0.0, 1.0, 0.0, 1.0), val / max);
    for (i = 0; i < num_squares - 1; i++)
    {
      CreateRect(sqr_position, sqr_size, white_pixel, white_pixel, color);
      sqr_position.x += sqr_size.x + padding;
    }

    CreateRect(sqr_position, glm::vec2(last_sqr_width, sqr_size.y),
               white_pixel, white_pixel,
               glm::mix(glm::vec4(1.0, 0.0, 0.0, 1.0),
                        glm::vec4(0.0, 1.0, 0.0, 1.0), (float)i / 10));

    // glm::vec2 half_point = position + (size * 0.25f);

    // CreateRect(position - glm::vec2(0.0, size.y + 2), size + glm::vec2(0.0, 2),
    // m_WhitePixelPos, m_WhitePixelPos, glm::vec4(0.0, 0.0, 0.0, 0.5));

    // Text(text, glm::vec2(half_point.x, position.y - size.y -
    // glm::ceil(highest_character * 0.2f)), glm::vec4(1.0f), false);
  }

  void UI::Scale(const std::string &text, int &current, int min, int max,
                 glm::vec2 fixed_size = glm::vec2(-1.0f))
  {
    auto font = m_Fonts[current_font];
    float widgetStart = m_pCursorPosition.x;
    CheckShader();
    if (Button("<"))
    {
      current--;
      current = current < min ? min : current;
    }

    current = glm::max(min, current);
    current = glm::clamp(current, 0, max - min);

    float size = m_pContext.prev_widget_size.x;

    Sameline();
    glm::vec2 position = m_pCursorPosition;

    std::string composite_text = text;

    auto text_size = textSize(composite_text);

    text_size = glm::ceil(text_size);

    auto btnSize = buttonSize(">");

    if (fixed_size.x > 0.0)
    {
      Text(composite_text,
           glm::ceil(glm::vec2(widgetStart, position.y) +
                     glm::vec2(fixed_size.x * 0.5f - text_size.x * 0.5f,
                               glm::ceil(font->highest_character * 0.1))),
           glm::vec4(1.0f), false);
    }
    else
    {
      Text(composite_text,
           position + glm::vec2(5.0, glm ::ceil(text_size.y * 0.75)),
           glm::vec4(1.0f), false);
    }

    // size += m_pContext.prev_widget_size.x;
    size += text_size.x;
    // Sameline();
    m_pCursorPosition.x += text_size.x;
    m_pCursorPosition.x += 15;

    if (fixed_size.x > 0.0)
    {

      m_pCursorPosition.x = (position.x + fixed_size.x) - btnSize.x - 36;
    }

    glm::vec2 white_pixel = glm::vec2(m_Fonts[current_font]->white_pixel.x, m_Fonts[current_font]->white_pixel.y);

    float span = (m_pCursorPosition.x - position.x) / (abs(max - min));
    for (int i = 0; i < glm::clamp(current, 0, max - min); i++)
    {
      CreateRect(position + glm::vec2((span * i), (btnSize.y)),
                 glm::vec2(span - (span * 0.2), 5), white_pixel,
                 white_pixel,
                 glm::mix(glm::vec4(1.0, 0.0, 0.0, 1.0),
                          glm::vec4(0.0, 1.0, 0.0, 1.0), (float)i / 10));
    }

    if (Button(">"))
    {
      current++;
      current = current > max ? max : current;
    }
    current = glm::max(min, current);
    current = glm::clamp(current, 0, max - min);

    size +=
        m_pContext.prev_widget_size.x +
        24; // 15 from text, 5 for widget padding, 2 on each side of text padding

    currentWindow->size.x =
        glm::max(m_pStyle.buttonPadding + size, currentWindow->size.x);

    if (fixed_size.x > 0.0)
      currentWindow->size.x = glm::max(fixed_size.x, currentWindow->size.x);

    m_pCursorPosition.x = currentWindow->position.x;
  }

  glm::vec2 UI::ToScreenCoords(const glm::mat4 &model, const glm::mat4 &view,
                               const glm::mat4 &proj,
                               const glm::vec2 &resolution)
  {
    glm::vec4 ndc = proj * view * model * glm::vec4(1.0f);

    ndc /= ndc.w;
    ndc.y = -1.0 * ndc.y;

    glm::vec2 viewport_space = glm::vec2(ndc.x, ndc.y) * 0.5f + 0.5f;

    return viewport_space * resolution;
  }
} // namespace UI