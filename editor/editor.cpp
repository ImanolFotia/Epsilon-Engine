
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/common.hpp>
#include <glm/gtx/hash.hpp>>

#include "editor.hpp"

#include "core/framework/IO/KeyBoard.hpp"
#include "ui/style.hpp"
#include "utils/mesh_utils.hpp"

namespace Editor {
void Editor::OnCreate() {

  m_MenuBar.addCallback("OnExit", [this]() { mShouldClose = true; });

  m_Camera = std::make_shared<utils::Camera>(
      glm::vec3(0.0, 5.0, 0.0), glm::vec3(0.0) - glm::vec3(5.0, 5.0, 4.5));

  m_Inspector.setTransform(selected_matrix);
  m_Inspector.setName("Cube");

  m_Scene = std::make_shared<engine::Scene>(getContext());

  m_ObjectProperty.m_pScenePtr = m_Scene;
  m_SceneNodes.m_pScenePtr = m_Scene;

  m_AssetManager = std::make_shared<engine::AssetManager>();
  m_AssetManager->setContext(getContext());
  m_AssetManager->Init();

  m_Scene->setAssetManager(m_AssetManager);

  m_GraphicsHelper =
      std::make_shared<GraphicsHelper>(getContext(), m_AssetManager);

  m_BrushManager = std::make_shared<BrushManager>(getContext());

  m_Assets = UI::Assets(m_Scene);

  engine::Cube cube;
  m_BrushManager->addBrush(
      toTriangleStrip(cube.data().Vertices, cube.data().Indices), 0);

  m_MaterialEditor.m_pSceneRef = m_Scene;

  host.assembly.Invoke<void>(L"setScenePtr", m_Scene.get());

  m_ComputeShader = ComputeDispatch::createComputeShader(m_Scene);

  m_ForwardRenderPass = Renderpasses::createForwardRenderPass(m_Scene);

  m_GraphicsHelper->addRenderPass(
      "DefaultRenderPass", Renderpasses::createDefaultRenderPass(m_Scene));

  m_TAAPasses = Renderpasses::createTAARenderPass(m_Scene);

  m_SSSPass = Renderpasses::createSSSRenderPass(m_Scene);

  // m_Scene->getAssetManager().CreateGPUBuffer("TAADataBuffer",
  // sizeof(Renderpasses::TAAUniformData),
  // engine::BufferStorageType::UNIFORM_BUFFER);

  m_GraphicsHelper->addRenderPass("Forward", m_ForwardRenderPass);

  m_GraphicsHelper->addRenderPass("TAARenderPass0", m_TAAPasses.renderpass[0]);
  m_GraphicsHelper->addRenderPass("TAARenderPass1", m_TAAPasses.renderpass[1]);

  m_GraphicsHelper->addRenderPass("SSSPass", m_SSSPass);

  m_GraphicsHelper->addBindGroup(
      "DefaultBindGroup", 0,
      {
          .bindingInfo = {{.size = sizeof(engine::PBRMaterial) *
                                   engine::AssetManager::MAX_MATERIALS,
                           .offset = 0,
                           .binding = 1,
                           .type = engine::UniformBindingType::SHADER_STORAGE,
                           .buffer = "material_buffer"}},
          .inputs = {},
          .renderPass = "Forward",
          .name = "DefaultBindGroup",
      });

  m_DefaultBindGroup = std::hash<std::string>{}("DefaultBindGroup");

  m_GraphicsHelper->addBindGroup("SkyBindGroup", 1,
                                 {
                                     .bindingInfo = {},
                                     .inputs = {},
                                     .renderPass = "Forward",
                                     .name = "SkyBindGroup",
                                 });

  m_SkyBindGroup = std::hash<std::string>{}("SkyBindGroup");

  m_GraphicsHelper->addBindGroup("GridBindGroup", 2,
                                 {
                                     .bindingInfo = {},
                                     .inputs = {},
                                     .renderPass = "Forward",
                                     .name = "GridBindGroup",
                                 });

  m_GridBindGroup = std::hash<std::string>{}("GridBindGroup");

  m_GraphicsHelper->addBindGroup(
      "GrassBindGroup", 3,
      {
          .bindingInfo = {{.size = sizeof(engine::PBRMaterial) *
                                   engine::AssetManager::MAX_MATERIALS,
                           .offset = 0,
                           .binding = 1,
                           .type = engine::UniformBindingType::SHADER_STORAGE,
                           .buffer = "material_buffer"},
                          {.size = sizeof(glm::mat4) *
                                   engine::AssetManager::MAX_TRANSFORMS,
                           .offset = 0,
                           .binding = 5,
                           .type = engine::UniformBindingType::SHADER_STORAGE,
                           .buffer = "transform_buffer"},
                          {.size = sizeof(engine::ShaderObjectData) *
                                   engine::AssetManager::MAX_OBJECTS,
                           .offset = 0,
                           .binding = 6,
                           .type = engine::UniformBindingType::SHADER_STORAGE,
                           .buffer = "object_buffer"}},
          .inputs = {},
          .renderPass = "Forward",
          .name = "GrassBindGroup",
      });

  m_GrassBindGroup = std::hash<std::string>{}("GrassBindGroup");

  m_GraphicsHelper->addBindGroup(
      "TAABindGroup0", 0,
      {
          .bindingInfo = {{.size = sizeof(Renderpasses::TAAUniformData),
                           .offset = 0,
                           .binding = 1,
                           .type = engine::UniformBindingType::UNIFORM_BUFFER,
                           .buffer = "TAADataBuffer",
                           .name = "TAADataBuffer"}},
          .inputs = {{.renderPass = "Forward", .index = 0, .bindingPoint = 2},
                     {.renderPass = "TAARenderPass1",
                      .index = 0,
                      .bindingPoint = 3},
                     {.renderPass = "Forward", .index = 1, .bindingPoint = 4},
                     {.renderPass = "Forward", .index = 2, .bindingPoint = 5}},
          .renderPass = "TAARenderPass0",
          .name = "TAABindGroup0",
      });

  m_GraphicsHelper->addBindGroup(
      "TAABindGroup1", 0,
      {
          .bindingInfo = {{.size = sizeof(Renderpasses::TAAUniformData),
                           .offset = 0,
                           .binding = 1,
                           .type = engine::UniformBindingType::UNIFORM_BUFFER,
                           .buffer = "TAADataBuffer",
                           .name = "TAADataBuffer"}},
          .inputs = {{.renderPass = "Forward", .index = 0, .bindingPoint = 2},
                     {.renderPass = "TAARenderPass0",
                      .index = 0,
                      .bindingPoint = 3},
                     {.renderPass = "Forward", .index = 1, .bindingPoint = 4},
                     {.renderPass = "Forward", .index = 2, .bindingPoint = 5}},
          .renderPass = "TAARenderPass1",
          .name = "TAABindGroup1",
      });

  m_TAABindGroup0 = std::hash<std::string>{}("TAABindGroup0");
  m_TAABindGroup1 = std::hash<std::string>{}("TAABindGroup1");

  m_GraphicsHelper->addBindGroup(
    "SSSBindGroup", 0,
    {
        .bindingInfo = {{.size = sizeof(Renderpasses::SSSUniformData),
                         .offset = 0,
                         .binding = 1,
                         .type = engine::UniformBindingType::UNIFORM_BUFFER,
                         .buffer = "SSSDataBuffer",
                         .name = "SSSDataBuffer"}},
        .inputs = {{.renderPass = "Forward", .index = 1, .bindingPoint = 2},
                   },
        .renderPass = "SSSRenderPass",
        .name = "SSSBindGroup",
    });

  m_SSSBindGroup = std::hash<std::string>{}("SSSBindGroup");
    

  auto renderer = getContext()->Renderer();

  renderer->InitDebugRenderer();
  auto imgui_renderer = renderer->getDebugRenderer();
  imgui_renderer->SetStyle(Catppuccin::GetPalette<Catppuccin::Mocha>());

  setup_style();

  auto node = Utils::CreateNode(glm::mat4(1.0f), m_Scene);
  Utils::AddCameraNode("Camera", m_Scene, node, {});

  m_SceneNodes.RegisterIntoEditor("Camera", node);

  auto skyNode = m_Scene->emplaceIntoScene<engine::Scene::SceneEntity>(
      engine::Box{glm::vec3(0.0f), glm::vec3(1.0)});
  skyNode->data.transform = glm::scale(glm::mat4(1.0), glm::vec3(3.0f));
  engine::Sphere sphere(5);
  auto SkyDomeNode =
      m_AssetManager->createModelFromMesh("Skybox", sphere.data(), {});
  m_Skybox = m_Scene->insertIntoNode(skyNode, SkyDomeNode);
  m_Skybox->data.bindGroupId = m_SkyBindGroup;

  ////////////////

  // setMaxFPS(72);

  auto planeNode = m_Scene->emplaceIntoScene<engine::Scene::SceneEntity>(
      engine::Box{glm::vec3(0.0f), glm::vec3(1.0)});

  planeNode->data.transform =
      glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0));
  planeNode->data.transform =
      glm::scale(planeNode->data.transform, glm::vec3(30.0, 1.0, 30.0));
  planeNode->data.transform = glm::rotate(
      planeNode->data.transform, glm::radians(270.f), glm::vec3(1.0, 0.0, 0.0));

  common::MeshMaterial defaultMaterial;
  defaultMaterial.roughness = 1.0;
  defaultMaterial.metallic = 0;
  defaultMaterial.color = glm::vec4(0.5, 0.5, 0.5, 1.0);
  engine::Quad quad(2, glm::vec2(5.0f), glm::vec3(2.0f));
  auto m_DefaultCube = m_AssetManager->createModelFromMesh(
      "DefaultPlane", quad.data(), defaultMaterial);
  m_GridPlane = m_Scene->insertIntoNode(
      engine::Box{glm::vec3(planeNode->data.transform[3]), glm::vec3(10.0)},
      planeNode, m_DefaultCube);
  m_GridPlane->data.bindGroupId = m_DefaultBindGroup;

  auto script = m_Scene->emplaceIntoNode<EntityScript>(planeNode);
  script->data.ManagedPtr = host.assembly.Invoke<void *>(
      L"CreateEntity", planeNode.get(), "Game.Ground", "Ground");

  script->data.updateClbk =
      host.assembly.getFunction<void, void *, float>(L"UpdateEntity");

  if (script->data.ManagedPtr == nullptr) {
    std::cerr << "ManagedPtr is null" << std::endl;
    exit();
  }

  const char *str = host.assembly.Invoke<const char *>(L"getEntityFields",
                                                       script->data.ManagedPtr);
  // std::cout << str << std::endl;
  auto props = UI::Property::DeserializeProperties(
      std::string(str)); // m_ObjectProperty.setProperties(std::string(str));
  script->data.properties = props;
  script->data.className = "Game.Ground";

  EntityArgs args;
  args.transform = toTransform(planeNode->data.transform);
  args.id = 0;
  host.assembly.Invoke<void>(L"setEntityTransform", script->data.ManagedPtr,
                             args);

  /////

  UI::NodeProperties node_props;
  node_props.name = "Ground";
  node_props.mType = UI::SceneNodeType::Node;

  UI::NodeProperties render_props;
  render_props.name = "Render Model" + std::to_string(m_GridPlane->Index());
  render_props.mType = UI::SceneNodeType::Render;
  render_props.node_ref = (void *)&m_GridPlane.get()->data;
  render_props.scene_node_ref = planeNode;
  render_props.index = planeNode->Index();

  UI::NodeProperties script_props;
  script_props.name = "Script";
  script_props.mType = UI::SceneNodeType::Script;
  script_props.node_ref = (void *)&script.get()->data;
  script_props.scene_node_ref = planeNode;
  script_props.index = planeNode->Index();

  node_props.children.push_back(render_props);
  node_props.node_ref = (void *)&planeNode.get()->data;
  node_props.model_path = "DefaultPlane";
  node_props.scene_node_ref = planeNode;
  node_props.index = planeNode->Index();
  m_SceneNodes.PushInRoot(node_props);

  //////////////////

  m_AssetManager->addTexture("textures/radiance.dds",
                             {.format = engine::TextureFormat::COLOR_RGBA,
                              .wrapMode = engine::CLAMP_TO_EDGE,
                              .filtering = engine::LINEAR});
  m_AssetManager->addTexture("textures/irradiance.dds",
                             {.format = engine::TextureFormat::COLOR_RGBA,
                              .wrapMode = engine::CLAMP_TO_EDGE,
                              .filtering = engine::LINEAR});
  std::ifstream file("./assets/lut.bin", std::ios::binary);
  size_t lut_size = 512 * 512 * sizeof(glm::vec2) * 2;
  unsigned char *lut = new unsigned char[lut_size];
  file.read((char *)lut, lut_size);
  m_AssetManager->addTextureFromBytes(
      "BRDF_lut", lut, lut_size, 512, 512, 2,
      {.format = engine::TextureFormat::COLOR_RG_32F,
       .wrapMode = engine::CLAMP_TO_EDGE,
       .filtering = engine::LINEAR});
  delete[] lut;

  pAddDefaultCube(glm::vec3(0.0));

  // for (int i = 0; i < 200; i++)
  //	for (int j = 0; j < 200; j++) {
  pAddDefaultPlane(glm::vec3(0.0f));
  //}

  renderer->getDebugRenderer()->ShowDebugPerformance(false);

  renderer->getDebugRenderer()->setUserFunction([this]() {
    m_MenuBar.draw();
    m_Tooldbar.draw();
    m_Inspector.draw();
    if (m_PostProcess.TaaEnabled() && !just_resized) {
      m_MainViewport.setImage(
          getContext()->Renderer()->getDebugRenderer()->getImages().at(
              m_CurrentTAAPass ? "TAATarget0" : "TAATarget1"));
    } else {
      m_MainViewport.setImage(
          getContext()->Renderer()->getDebugRenderer()->getImages().at(
              "Forward0"));
      just_resized = false;
    }
    m_MainViewport.draw();
    m_SceneNodes.draw();
    m_Assets.draw();
    m_PostProcess.draw();
    m_ThemeSelector.draw();
    engine::Node<engine::Scene::SceneEntity> *sn;
    if (m_SceneNodes.scene_node_ref != nullptr) {
      sn = static_cast<engine::Node<engine::Scene::SceneEntity> *>(
          m_SceneNodes.scene_node_ref);
      selected_entity =
          &sn->data; // reinterpret_cast<std::shared_ptr<engine::Scene::SceneEntity>>
                     // (m_SceneNodes.scene_node_ref);
    }
    selected_index = m_SceneNodes.selected_index;
    m_MaterialEditor.selected_entity = selected_index;
    m_MaterialEditor.draw();

    glm::mat4 original_transform = m_Inspector.getTransform();
    selected_matrix = m_Inspector.getTransform();
    if (selected_entity != nullptr)
      m_Guizmo.prepare(&selected_matrix, m_MainViewport.getMode(),
                       m_Camera->getViewMatrix(),
                       m_Camera->getProjectionMatrix());

    if (m_MainViewport.getMode() != UI::NONE && !m_Navigation &&
        selected_entity != nullptr) {
      m_Guizmo.draw();
    }

    if (original_transform != selected_matrix && selected_entity != nullptr) {
      auto e =
          std::static_pointer_cast<engine::Node<engine::Scene::SceneEntity>>(
              m_Scene->getNode(selected_index));
      EntityArgs args;
      args.transform = toTransform(selected_matrix);
      e->data.transform = selected_matrix;
      args.id = 0;
    }

    if (selected_index > 0) {
      auto e = m_Scene->getNode(selected_index);
      auto children = m_Scene->getChildren(e);
      m_ObjectProperty.SetNode(m_SceneNodes.selected_node);

      bool containsScript = false;
      for (auto &child : children) {

        if (child.first == typeid(EntityScript).hash_code()) {
          containsScript = true;
          break;
        }
      }

      if (containsScript) {

        auto s = m_Scene->getChild<EntityScript>(e);
        auto script = std::static_pointer_cast<engine::Node<EntityScript>>(s);
        m_ObjectProperty.setProperties(s->data.className,
                                       script->data.properties);
        m_ObjectProperty.draw();

        for (auto &property : m_ObjectProperty.getProperties()) {
          std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

          std::wstring name = converter.from_bytes(property.name);
          if (property.type == UI::PropertyType::NUMBER) {
            host.assembly.Invoke<void>(L"SetProperty", script->data.ManagedPtr,
                                       property.name.c_str(),
                                       std::any_cast<float>(property.value));
          }
          if (property.type == UI::PropertyType::BOOL) {
            host.assembly.Invoke<void>(
                L"SetPropertyBool", script->data.ManagedPtr,
                property.name.c_str(), std::any_cast<bool>(property.value));
          }
        }
        script->data.properties = m_ObjectProperty.getProperties();
      } else {

        m_ObjectProperty.draw();
      }
    }

    ImGui::Begin("Reload Assembly");

    if (ImGui::Button(ICON_FA_REDO, ImVec2(37, 37))) {
      host.assembly.Invoke<void>(L"ReloadAssemblies");

      auto scripts = m_Scene->getNodes<EntityScript>();
      for (auto s : scripts) {
        auto script = std::static_pointer_cast<engine::Node<EntityScript>>(s);
        const char *str = host.assembly.Invoke<const char *>(
            L"getEntityFields", script->data.ManagedPtr);
        auto props = UI::Property::DeserializeProperties(std::string(str));
        script->data.properties = props;
      }
    }

    ImGui::SameLine();
    ImGui::End();
  });
}

void Editor::OnUpdate() {

  if (m_Tooldbar.getStatus() != UI::Toolbar::STATUS::Paused ||
      m_Tooldbar.GetStepStatus() == UI::Toolbar::StepStatus::Next) {

    tiny_keyboard kb{};

    // std::memcpy(kb.keys, framework::Input::KeyBoard::KEYS, sizeof(int) *
    // 1024);
    kb.convert();

    // host.assembly.Invoke<void>(L"setKeyboardState", kb);

    auto models = m_Scene->getNodes<engine::RenderModel>();
    auto scripts = m_Scene->getNodes<EntityScript>();

    for (auto s : scripts) {
      auto script_node =
          std::static_pointer_cast<engine::Node<EntityScript>>(s);
      auto &script = script_node->data;
      auto transform =
          std::static_pointer_cast<engine::Node<engine::Scene::SceneEntity>>(
              script_node->Parent());

      if (m_Tooldbar.getStatus() == UI::Toolbar::STATUS::Playing) {
        EntityArgs args;
        args.transform = toTransform(transform->data.transform);
        args.id = 0;
        host.assembly.Invoke<void>(L"setEntityTransform", script.ManagedPtr,
                                   args);

        script.Update((float)framework::Clock::DeltaSeconds());
      } else {

        EntityArgs args;
        args.transform = toTransform(transform->data.transform);
        args.id = 0;
        host.assembly.Invoke<void>(L"setEntityTransform", script.ManagedPtr,
                                   args);
      }
    }

    if (selected_entity != nullptr) {
      selected_matrix = selected_entity->transform;
      m_Inspector.setTransform(selected_entity->transform);
    }

    m_Tooldbar.SetStepStatus(UI::Toolbar::StepStatus::Current);
  }

  int screenX, screenY; // getWindowDimensions();
  screenX = m_MainViewport.getSize().x;
  screenY = m_MainViewport.getSize().y;
  shaderData.iResolution = glm::vec2(screenX, screenY);

  range += 0.001f;
  range = glm::mod(range, 2.0f);

  m_Camera->UpdateMatrices(Frame(), screenX, screenY,
                           m_PostProcess.TaaEnabled());

  if (framework::Input::Mouse::MIDDLE == framework::Input::PRESSED &&
      m_MainViewport.isHovered()) {
    if (m_CameraWasInactive) {
      m_Camera->ResetDeltas();
      m_CameraWasInactive = false;
    }
    // getContext()->Window().HideCursor();
    m_Camera->Update(Epsilon::getContext()->Window().getWindow());
    m_Navigation = true;
  } else {
    // getContext()->Window().ShowCursor();
    m_CameraWasInactive = true;
    m_Navigation = false;
  }

  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  auto time = std::chrono::duration<float, std::chrono::seconds::period>(
                  currentTime - startTime)
                  .count();

  shaderData.prev_view = m_Camera->getPrevViewMatrix();
  shaderData.prev_proj = shaderData.proj;
  shaderData.view = m_Camera->getViewMatrix();
  shaderData.proj = m_Camera->getProjectionMatrix();
  shaderData.proj[1][1] *= -1;
  shaderData.iTime = framework::Clock::TimeSeconds();
  shaderData.viewPosition = m_Camera->getPosition();
  shaderData.iFrame = Frame();

  getContext()->Renderer()->UpdateRenderPassUniforms(
      m_ForwardRenderPass, engine::RENDERPASS_SET, &shaderData);

  if (m_PostProcess.TaaEnabled()) {
    getContext()->Renderer()->UpdateRenderPassUniforms(
        m_TAAPasses.renderpass[0], engine::RENDERPASS_SET, &shaderData);
    getContext()->Renderer()->UpdateRenderPassUniforms(
        m_TAAPasses.renderpass[1], engine::RENDERPASS_SET, &shaderData);
    getContext()->Renderer()->UpdateRenderPassUniforms(
        m_TAAPasses.renderpass[0], (engine::BindingIndex)1,
        &m_PostProcess.m_TAAData);
    getContext()->Renderer()->UpdateRenderPassUniforms(
        m_TAAPasses.renderpass[1], (engine::BindingIndex)1,
        &m_PostProcess.m_TAAData);
  }

  if (m_MainViewport.ShouldResize()) {

    getContext()->ResourceManager()->ResizeFramebuffer(
        m_ForwardRenderPass, m_MainViewport.getSize());
    if (m_PostProcess.TaaEnabled()) {
      getContext()->ResourceManager()->ResizeFramebuffer(
          m_TAAPasses.renderpass[0], m_MainViewport.getSize());
      getContext()->ResourceManager()->ResizeFramebuffer(
          m_TAAPasses.renderpass[1], m_MainViewport.getSize());
      getContext()->ResourceManager()->updateBindGroup(
          m_GraphicsHelper->getBindGroups()[m_TAABindGroup1].bindGroup, {});
      getContext()->ResourceManager()->updateBindGroup(
          m_GraphicsHelper->getBindGroups()[m_TAABindGroup0].bindGroup, {});
      just_resized = true;
    }
    Epsilon::getContext()
        ->Renderer()
        ->getDebugRenderer()
        ->recreateDescriptorSets();
    m_MainViewport.ResetFlags();
  }
  glm::ivec3 cam_coords = glm::floor(m_Camera->getPosition());
  if (prev_cam_coords.x != cam_coords.x || prev_cam_coords.z != cam_coords.z) {

    auto grass = m_Scene->getNode(m_GrassNodeIndex);
    auto grass_model_node = m_Scene->getChild<engine::RenderModel>(grass);
    const int tile_size = 30;
    const int num_tiles = 200 / 30;

    glm::ivec4 current_coords =
        glm::ivec4(-15 + cam_coords.x, -15 + cam_coords.z,
                   (200.0f * 0.15) - 15 + cam_coords.x,
                   (200.0f * 0.15) - 15 + cam_coords.z);

    std::mt19937 e2(555);

    int offset_i = prev_cam_coords.x > cam_coords.x   ? -1
                   : prev_cam_coords.x < cam_coords.x ? 1
                                                      : 0;
    int offset_j = prev_cam_coords.z > cam_coords.z   ? -1
                   : prev_cam_coords.z < cam_coords.z ? 1
                                                      : 0;
    prev_cam_coords = cam_coords;

    size_t row_size = 30;

    struct check_info {
      int index = 0;
      bool checked = false;
    };
    std::vector<int> unused_chunks;
    std::unordered_map<glm::ivec2, check_info> old_chunks{};
    std::unordered_map<glm::ivec2, check_info> new_chunks{};
    for (int i = 0; i < row_size; i++) {
      for (int j = 0; j < row_size; j++) {
        int index = i + j * row_size;
        auto &chunk = grass_chunks[index];
        old_chunks[glm::ivec2(chunk.id_x, chunk.id_y)] = {chunk.index, false};
      }
    }

    for (int i = 0; i < row_size; i++) {
      for (int j = 0; j < row_size; j++) {
        int index = i + j * row_size;
        auto &chunk = grass_chunks[index];
        auto coord = glm::ivec2(i + cam_coords.x, j + cam_coords.z);

        if (!old_chunks.contains(coord))
          new_chunks[glm::ivec2(i + cam_coords.x, j + cam_coords.z)] = {
              chunk.index, false};
        else
          old_chunks[coord].checked = true;
      }
    }

    for (auto &[key, value] : old_chunks) {
      if (!value.checked)
        unused_chunks.push_back(value.index);
    }

    int i = 0;
    for (auto &[key, value] : new_chunks) {
      value.index = unused_chunks[i++];
    }
    for (auto &[key, value] : new_chunks) {
      auto &chunk = grass_chunks.at(value.index);
      chunk.id_x = key.x;
      chunk.id_y = key.y;
      

      float dist = glm::distance(glm::vec2(key.x, key.y) - 15.0f, glm::vec2(cam_coords.x, cam_coords.z));

      for (int index = 0; index < chunk.transform_count; index++) {
        auto &transform =
            grass_model_node->data.transforms[chunk.transforms[index]];
        {
          glm::vec3 position =
              glm::vec3((float)key.x - 15, 0.0f, (float)key.y - 15);

          std::uniform_real_distribution<float> dist(0.0, 1.0);
          std::uniform_real_distribution<float> dist2(-glm::pi<float>(),
                                                      glm::pi<float>());

          glm::mat4 transform_matrix = glm::translate(
              glm::mat4(1.0), position + glm::vec3(dist(e2), 0.0, dist(e2)));
          glm::mat4 rotation_matrix =
              glm::rotate(glm::mat4(1.0), dist2(e2), glm::vec3(0.0, 1.0, 0.0));
          glm::mat4 model_matrix = transform_matrix * rotation_matrix;
          transform = model_matrix;
        }
      }
    }

    // for (auto &transform : grass_model_node->data.transforms) {
    /*
        for (auto &c : invalid_indices) {
          auto *chunk = c;
          float i = chunk->id_x;
          float j = chunk->id_y;
          for (int index = 0; index < chunk->transform_count; index++) {
            auto &transform =
                grass_model_node->data.transforms[chunk->transforms[index]];
            {
              glm::vec3 position = glm::vec3((float)i-1.5, 0.0f, (float)j-1.5);

              std::uniform_real_distribution<float> dist(0.0, 1.0);
              std::uniform_real_distribution<float> dist2(-glm::pi<float>(),
                                                          glm::pi<float>());

              glm::mat4 transform_matrix = glm::translate(
                  glm::mat4(1.0), position + glm::vec3(dist(e2), 0.0,
       dist(e2))); glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0),
       dist2(e2), glm::vec3(0.0, 1.0, 0.0)); glm::mat4 model_matrix =
       transform_matrix * rotation_matrix; transform = model_matrix;
            }
          }
        }*/
  }
}

void Editor::OnRender() {

  m_Scene->BeginScene();

  // m_Scene->getContext()->Renderer()->ComputeDispatch(m_ComputeShader);

  m_Scene->setCurrentRenderPass(m_GraphicsHelper->getRenderPass("Forward"));

  auto models = m_Scene->getNodes<engine::RenderModel>();

  for (auto &model_node : models) {
    auto model =
        std::static_pointer_cast<engine::Node<engine::RenderModel>>(model_node);
    auto transform =
        std::static_pointer_cast<engine::Node<engine::Scene::SceneEntity>>(
            model->Parent());
    if (model->data.isInstanced) {
      m_Scene->Push(model, model->data.transforms,
                    model->data.transforms.size(),
                    m_GraphicsHelper->getBindGroup(model->data.bindGroupId));
    } else {
      m_Scene->Push(model, transform->data.transform,
                    m_GraphicsHelper->getBindGroup(model->data.bindGroupId));
    }
  }

  m_Scene->Flush();

  if (m_PostProcess.TaaEnabled()) {
    m_Scene->setCurrentRenderPass(
        /**/ m_GraphicsHelper->getRenderPass(
            m_CurrentTAAPass ? "TAARenderPass0" : "TAARenderPass1"));

    m_Scene->Push(m_GridPlane, glm::mat4(1.0),
                  m_GraphicsHelper->getBindGroup(
                      m_CurrentTAAPass ? m_TAABindGroup0 : m_TAABindGroup1));

    m_CurrentTAAPass = !m_CurrentTAAPass;
    m_Scene->Flush();
  }

  m_Scene->EndScene();
}

void Editor::pAddDefaultPlane(glm::vec3 position) {

  common::MeshMaterial defaultMaterial;
  defaultMaterial.roughness = 0.34;
  defaultMaterial.metallic = 0;
  defaultMaterial.specular = 0.5;
  defaultMaterial.color = glm::vec4(0.15, 0.6, 0.15, 1.0);
  defaultMaterial.normal_path = "textures/grass_normal.png";
  defaultMaterial.name = "grass_material";

  engine::Quad grass_lod[3] = {engine::Quad(4, glm::vec2(1.0 / 4.0, 1.0)),
                               engine::Quad(3, glm::vec2(0.3f, 1.0)),
                               engine::Quad(2, glm::vec2(0.5f, 1.0))};

  for (int lod = 0; lod < 3; lod++) {
    for (auto &vtx : grass_lod[lod].data().Vertices) {
      float tmp = vtx.position.y;
      vtx.position.y = -vtx.position.z;
      vtx.position.z = tmp;

      tmp = vtx.normal.y;
      vtx.normal.y = vtx.normal.z;
      vtx.normal.z = tmp;

      vtx.position.x = vtx.position.x * 2.0f;

      vtx.position.y += 0.5;
    }

    for (auto &vtx : grass_lod[lod].data().Vertices) {

      vtx.position.x *= 0.125f;
      vtx.position.x += 0.05f;
      float y = vtx.position.y;
      vtx.position.x *= 1.0 - (y * y * y * y);
      vtx.position.x *= 0.5f;
    }

    grass_lod[lod].generateTangentSpaceVectors();
  }

  auto GrassBlade_Lod0 = m_AssetManager->createModelFromMesh(
      "Grass Blade Lod0", grass_lod[0].data(), defaultMaterial);
  auto GrassBlade_Lod1 = m_AssetManager->createModelFromMesh(
      "Grass Blade Lod1", grass_lod[1].data(), defaultMaterial);
  auto GrassBlade_Lod2 = m_AssetManager->createModelFromMesh(
      "Grass Blade Lod2", grass_lod[2].data(), defaultMaterial);

  GrassBlade_Lod0.renderMeshes[1] = GrassBlade_Lod1.renderMeshes[0];
  GrassBlade_Lod0.renderMeshes[2] = GrassBlade_Lod2.renderMeshes[0];
  GrassBlade_Lod0.isInstanced = true;

  for (int i = 0; i < 30; i++) {
    for (int j = 0; j < 30; j++) {
      grass_chunks.emplace_back();
    }
  }

  std::mt19937 e2(555);
  for (int i = 0; i < 30; i++) {
    for (int j = 0; j < 30; j++) {
      // pAddDefaultPlane(glm::vec3((float)(i * 0.15) - 15, 0.0f, (float)(j *
      // 0.15) - 15));
      int index = i + j * 30;

      auto &chunk = grass_chunks.at(index);
      chunk.id_x = i;
      chunk.id_y = j;
      chunk.index = index;

      for (int k = 0; k < 300; k++) {
        glm::vec3 position = glm::vec3((float)i - 15, 0.0f, (float)j - 15);
        std::random_device rd;

        std::uniform_real_distribution<float> dist(0.0, 1.0);
        std::uniform_real_distribution<float> dist2(-glm::pi<float>(),
                                                    glm::pi<float>());

        glm::mat4 transform_matrix = glm::translate(
            glm::mat4(1.0), position + glm::vec3(dist(e2), 0.0, dist(e2)));
        glm::mat4 rotation_matrix =
            glm::rotate(glm::mat4(1.0), dist2(e2), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 model_matrix = transform_matrix * rotation_matrix;
        GrassBlade_Lod0.transforms.push_back(model_matrix);

        chunk.transforms[chunk.transform_count] =
            GrassBlade_Lod0.transforms.size() - 1;
        chunk.transform_count++;
      }
    }
  }

  auto node = Utils::CreateNode(glm::mat4(1.0), m_Scene);
  m_GrassNodeIndex = node->Index();

  Utils::AddModelNode("Grass Blades" + std::to_string(node->Index()), m_Scene,
                      GrassBlade_Lod0, node, "GrassBindGroup");

  Utils::AddScriptNode({.language = C_SHARP,
                        .fileName = "GameObject.cs",
                        .assemblyName = "Game.dll",
                        .className = "Game.GameObject",
                        .nodeName = "Plane." + node->Index()},
                       m_Scene, node, host);

  m_SceneNodes.RegisterIntoEditor("Grass", node);
}

void Editor::pAddDefaultCube(glm::vec3 position) {

  common::MeshMaterial defaultMaterial;
  defaultMaterial.roughness = 0.8;
  defaultMaterial.metallic = 0;
  defaultMaterial.color = glm::vec4(0.5, 0.5, 0.5, 1.0);

  engine::Cube cube;
  auto m_DefaultCube = m_AssetManager->createModelFromMesh("DefaultCube",
                            cube.data(), defaultMaterial);

  auto node = Utils::CreateNode(glm::mat4(1.0f), m_Scene);

  Utils::AddModelNode("Cube", m_Scene, m_DefaultCube, node);

  Utils::AddScriptNode({.language = C_SHARP,
                        .fileName = "GameObject.cs",
                        .assemblyName = "Game.dll",
                        .className = "Game.GameObject",
                        .nodeName = "Cube"},
                       m_Scene, node, host);

  m_SceneNodes.RegisterIntoEditor("Cube", node);
}
} // namespace Editor