//
// Created by solaire on 12/22/22.
//

#include "chess.hpp"
#include "game/common.hpp"
#include <core/framework/IO/IO.hpp>

namespace ChessApp
{

    void ChessApp::onCreate()
    {

        using namespace engine;
        using namespace framework;

        m_pMoveAudioObject.audioFile.Load("./assets/audio/move.wav");

        m_pMoveAudioObject.buffer = al::createBuffer(m_pMoveAudioObject.audioFile.getNumberOfChannels(),
                                                     m_pMoveAudioObject.audioFile.getFileSize(),
                                                     m_pMoveAudioObject.audioFile.getBPS(),
                                                     m_pMoveAudioObject.audioFile.getSampleRate(),
                                                     m_pMoveAudioObject.audioFile.data().get());

        m_pMoveAudioObject.source = al::createSource(m_pMoveAudioObject.buffer);

        m_pTakeAudioObject.audioFile.Load("./assets/audio/take.wav");

        m_pTakeAudioObject.buffer = al::createBuffer(m_pTakeAudioObject.audioFile.getNumberOfChannels(),
                                                     m_pTakeAudioObject.audioFile.getFileSize(),
                                                     m_pTakeAudioObject.audioFile.getBPS(),
                                                     m_pTakeAudioObject.audioFile.getSampleRate(),
                                                     m_pTakeAudioObject.audioFile.data().get());

        m_pTakeAudioObject.source = al::createSource(m_pTakeAudioObject.buffer);

        auto files = m_pBoard.getFiles();

        setupRenderPass();
        setupGeometry();
        setupTextures();

        Input::Mouse::MouseEventHandler += ([this](auto *sender, beacon::args *args)
                                            {
            if (args == nullptr)
                return;

            if (m_pUCI.mate) return;

                auto obj = args->to<Input::MouseArgs>();

            auto &files = m_pBoard.getFiles();

            auto calcRowCol = [this](int x, int y) -> std::pair<int, int> {
                glm::vec2 init_position;
                init_position.x = x - ((getWindowDimensions().first / 2) - board_size());
                init_position.y = (getWindowDimensions().second - y);

                int row = (init_position.y / (piece_size() * 2)) + 1;
                int col = (int) glm::floor(init_position.x / (piece_size() * 2));

                return {row, col};
            };

            auto printCharacter = [this](int x, int y) -> std::string {
                return "abcdefgh"[y] + std::to_string(x);
            };


            if (obj.Left().State == Input::PRESSED) {
                std::pair<int, int> position = calcRowCol(obj.X(), obj.Y());
                std::string pos = printCharacter(position.first, position.second);

                if (files[pos].piece == FREE) {
                    m_pMove.selected = false;
                    return;
                }

                m_pMove.selected = true;
                m_pMove.first[0] = pos.at(0);
                m_pMove.first[1] = pos.at(1);
                m_pMove.row_from = position.first;
                m_pMove.column_from = (COLUMN) position.second;

                if (files[pos].column == m_pMove.column_from && files[pos].row == m_pMove.row_from) {
                    if (m_pBoard.Turn() == WHITE && files[pos].piece > 5 ||
                        m_pBoard.Turn() == BLACK && files[pos].piece < 6) {
                        m_pMove.selected = false;
                        return;
                    }
                    if (files[pos].index >= 0 && files.contains(pos))
                        m_pSelectedModel = &m_pPieces[files[pos].index];
                }
            }

            if (framework::Input::Mouse::LEFT == framework::Input::STATE::PRESSED) {
                if (m_pSelectedModel != nullptr) {
                    m_pSelectedModel->pushConstant.model = glm::translate(glm::mat4(1.0),
                                                                          glm::vec3(-(obj.Y() -
                                                                                      getWindowDimensions().second *
                                                                                      0.5),
                                                                                    -(obj.X() -
                                                                                      getWindowDimensions().first *
                                                                                      0.5),
                                                                                    -498.0));

                    m_pSelectedModel->pushConstant.model = glm::scale(m_pSelectedModel->pushConstant.model,
                                                                      glm::vec3(piece_size(), piece_size(), 0.0));
                }

            }

            if (obj.Left().State == Input::RELEASED) {
                if (!m_pMove.selected) return;

                m_pMove.selected = false;

                std::pair<int, int> position = calcRowCol(obj.X(), obj.Y());
                std::string second = printCharacter(position.first, position.second);

                if (m_pMove.first[0] == second[0] && m_pMove.first[1] == second[1]) {
                    if (m_pSelectedModel != nullptr) {
                        m_pSelectedModel->pushConstant.model = transformPiece({m_pMove.column_from, m_pMove.row_from});
                        m_pSelectedModel = nullptr;
                        m_pMove.selected = false;
                        return;
                    }
                }

                m_pMove.second[0] = second.at(0);
                m_pMove.second[1] = second.at(1);
                m_pMove.row_to = position.first;
                m_pMove.column_to = (COLUMN) position.second;

                std::string strFirst = std::string(m_pMove.first, 2);
                std::string strSecond = std::string(m_pMove.second, 2);

                if (m_pMove.column_to > H || m_pMove.row_to > 8 || m_pMove.column_to < 0 || m_pMove.row_to < 0) {
                    if (m_pSelectedModel != nullptr) {
                        m_pSelectedModel->pushConstant.model = transformPiece({m_pMove.column_from, m_pMove.row_from});
                        return;
                    }
                }

                if (files[strSecond].piece != FREE) {

                    PIECE this_piece = files[strFirst].piece;
                    PIECE taken_piece = files[strSecond].piece;

                    if (taken_piece < 6 && this_piece < 6 || taken_piece > 5 && this_piece > 5) {

                        m_pSelectedModel->pushConstant.model = transformPiece(
                                {m_pMove.column_from, m_pMove.row_from});
                        return;
                    }

                    m_pTakeAudioObject.should_play = true;
                    std::cout << "piece taken\n";
                }

                files[strSecond].column = m_pMove.column_to;
                files[strSecond].row = m_pMove.row_to;
                files[strSecond].piece = files[strFirst].piece;
                files[strSecond].index = files[strFirst].index;
                files[strSecond].show = true;

                files[strFirst].column = m_pMove.column_from;
                files[strFirst].row = m_pMove.row_from;
                files[strFirst].index = -1;
                files[strFirst].show = false;
                files[strFirst].piece = FREE;

                if (files[strSecond].piece == BLACK_KING || files[strSecond].piece == WHITE_KING)
                {
                    if (m_pBoard.Turn() == WHITE)
                    {
                        if (!m_pBoard.WhiteCastled())
                            if (files[strSecond].column == COLUMN::G)
                            {

                                files["f1"].piece = WHITE_ROOK;
                                files["f1"].index = files["h1"].index;
                                files["f1"].column = F;
                                files["f1"].show = true;
                                files["f1"].row = 1;

                                files["h1"].piece = FREE;
                                files["h1"].index = -1;
                                files["h1"].column = H;
                                files["h1"].row = 1;
                                files["h1"].show = false;

                                m_pPieces.at(files["f1"].index).pushConstant.model = transformPiece(
                                    {files["f1"].column, files["f1"].row});

                                std::cout << "Short white castle" << std::endl;
                            }
                            else if (files[strSecond].column == COLUMN::C)
                            {

                                files["d1"].piece = WHITE_ROOK;
                                files["d1"].index = files["a1"].index;
                                files["d1"].column = D;
                                files["d1"].show = true;
                                files["d1"].row = 1;

                                files["a1"].piece = FREE;
                                files["a1"].index = -1;
                                files["a1"].column = H;
                                files["a1"].row = 1;
                                files["a1"].show = false;

                                m_pPieces.at(files["d1"].index).pushConstant.model = transformPiece(
                                    {files["d1"].column, files["d1"].row});

                                std::cout << "Long white castle" << std::endl;
                            }
                    }
                    else
                    {

                        if (!m_pBoard.BlackCastled())
                            if (files[strSecond].column == COLUMN::G)
                            {
                                std::cout << "Short black castle" << std::endl;
                            }
                            else
                            {
                                std::cout << "Long black castle" << std::endl;
                            }
                    }
                }

                m_pPieces.at(files[strSecond].index).pushConstant.model = transformPiece(
                        {files[strSecond].column, files[strSecond].row});
                m_pSelectedModel = nullptr;

                m_pBoard.Turn(m_pBoard.Turn() == WHITE ? BLACK : WHITE);
                m_pBoard.addMove(std::string(m_pMove.first, 2) + std::string(m_pMove.second, 2));
                std::cout << "from: " << m_pMove.first[0] << m_pMove.first[1] << " to: " << m_pMove.second[0]
                          << m_pMove.second[1]
                          << std::endl;
                std::string move_str = "position startpos moves ";
                for (const auto &move: m_pBoard.getPlayedMoves())
                    move_str += move + " ";

                m_pUCI.Move(move_str);
                m_pMoveAudioObject.should_play = true;

            } });
    }

    void ChessApp::onReady()
    {
        m_pEngineThread = std::thread([this]()
                                      {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            m_pUCI.init(); });

        m_pEngineThread.detach();
    }

    void ChessApp::onRender()
    {

        Epsilon::getContext().Renderer()->Begin();
        if (m_pTakeAudioObject.should_play)
        {
            al::playSource(m_pTakeAudioObject.source);
            m_pTakeAudioObject.should_play = false;
            m_pMoveAudioObject.should_play = false;
        }
        else if (m_pMoveAudioObject.should_play)
        {
            al::playSource(m_pMoveAudioObject.source);
            m_pMoveAudioObject.should_play = false;
        }
        // Set up the camera data
        setupCamera();
        engine::ObjectData objectData;

        m_pBoardModel.pushConstant.model = transformBoard(glm::vec3(0.0, 0.0, -500.0),
                                                          glm::vec3(board_size(), board_size(), 0.0));
        objectData.layout_index = 0;
        objectData.mesh = m_pBoardModel.mesh;
        objectData.material = m_pBoardModel.material;
        objectData.modelMatrix = m_pBoardModel.pushConstant.model;
        objectData.pushConstant = m_pBoardModel.pushConstantRef;
        Epsilon::getContext().Renderer()->Push(objectData);

        // set up the pieces draw data

        for (auto &[position, info] : m_pBoard.getFiles())
        {
            if (!info.show || info.index == -1)
                continue;
            auto &piece = m_pPieces.at(info.index);
            piece.pushConstant.piece = info.piece;

            objectData.layout_index = 1;
            objectData.mesh = piece.mesh;
            objectData.material = piece.material;
            objectData.pushConstant = piece.pushConstantRef;
            Epsilon::getContext().Renderer()->Push(objectData);
        }

        engine::Context::getSingleton().Renderer()->Flush(m_pRenderPass, engine::DrawType::INDEXED);
        engine::Context::getSingleton().Renderer()->End();
        engine::Context::getSingleton().Renderer()->Sync();
        // drawFrame(m_pRenderPass);

        if (!m_pUCI.last_move.empty() && m_pBoard.Turn() == BLACK)
        {

            std::cout << "black turn\n";

            auto getColumn = [](char col) -> COLUMN
            {
                if (col == 'a')
                    return A;
                if (col == 'b')
                    return B;
                if (col == 'c')
                    return C;
                if (col == 'd')
                    return D;
                if (col == 'e')
                    return E;
                if (col == 'f')
                    return F;
                if (col == 'g')
                    return G;
                if (col == 'h')
                    return H;
            };

            auto getRow = [](char row) -> char
            {
                if (row == '1')
                    return 1;
                if (row == '2')
                    return 2;
                if (row == '3')
                    return 3;
                if (row == '4')
                    return 4;
                if (row == '5')
                    return 5;
                if (row == '6')
                    return 6;
                if (row == '7')
                    return 7;
                if (row == '8')
                    return 8;
                return ENUM_MAX;
            };
            auto &files = m_pBoard.getFiles();
            std::string strFirst = m_pUCI.last_move.substr(0, 2);
            std::string strSecond = m_pUCI.last_move.substr(2, 2);

            if (files[strSecond].piece != FREE)
            {

                PIECE this_piece = files[strFirst].piece;
                PIECE taken_piece = files[strSecond].piece;

                if (taken_piece < 6 && this_piece < 6 || taken_piece > 5 && this_piece > 5)
                {

                    m_pSelectedModel->pushConstant.model = transformPiece(
                        {m_pMove.column_from, m_pMove.row_from});
                    return;
                }

                m_pTakeAudioObject.should_play = true;
            }

            m_pMove.selected = false;
            m_pMove.first[0] = strFirst.at(0);
            m_pMove.first[1] = strFirst.at(1);
            m_pMove.row_from = getRow(strFirst.at(1));
            m_pMove.column_from = (COLUMN)getColumn(strFirst.at(0));

            m_pMove.selected = false;
            m_pMove.second[0] = strSecond.at(0);
            m_pMove.second[1] = strSecond.at(1);
            m_pMove.row_to = getRow(strSecond.at(1));
            m_pMove.column_to = (COLUMN)getColumn(strSecond.at(0));

            files[strSecond].column = m_pMove.column_to;
            files[strSecond].row = m_pMove.row_to;
            files[strSecond].piece = files[strFirst].piece;
            files[strSecond].index = files[strFirst].index;

            if (files[strSecond].piece == BLACK_KING || files[strSecond].piece == WHITE_KING)
            {
                if (m_pBoard.Turn() == WHITE)
                {
                    if (!m_pBoard.WhiteCastled())
                        if (files[strSecond].column == COLUMN::G)
                        {

                            files["f1"].piece = WHITE_ROOK;
                            files["f1"].index = files["h1"].index;
                            files["f1"].column = F;
                            files["f1"].show = true;
                            files["f1"].row = 1;

                            files["h1"].piece = FREE;
                            files["h1"].index = -1;
                            files["h1"].column = H;
                            files["h1"].row = 1;
                            files["h1"].show = false;

                            m_pPieces.at(files["f1"].index).pushConstant.model = transformPiece(
                                {files["f1"].column, files["f1"].row});

                            std::cout << "Short white castle" << std::endl;
                        }
                        else if (files[strSecond].column == COLUMN::C)
                        {

                            files["d1"].piece = WHITE_ROOK;
                            files["d1"].index = files["a1"].index;
                            files["d1"].column = D;
                            files["d1"].show = true;
                            files["d1"].row = 1;

                            files["a1"].piece = FREE;
                            files["a1"].index = -1;
                            files["a1"].column = H;
                            files["a1"].row = 1;
                            files["a1"].show = false;

                            m_pPieces.at(files["d1"].index).pushConstant.model = transformPiece(
                                {files["d1"].column, files["d1"].row});

                            std::cout << "Long white castle" << std::endl;
                        }
                }
                else
                {

                    if (!m_pBoard.BlackCastled())
                        if (files[strSecond].column == COLUMN::G)
                        {

                            files["f8"].piece = BLACK_ROOK;
                            files["f8"].index = files["h8"].index;
                            files["f8"].column = F;
                            files["f8"].show = true;
                            files["f8"].row = 8;

                            files["h8"].piece = FREE;
                            files["h8"].index = -1;
                            files["h8"].column = H;
                            files["h8"].row = 8;
                            files["h8"].show = false;

                            m_pPieces.at(files["f8"].index).pushConstant.model = transformPiece(
                                {files["f8"].column, files["f8"].row});

                            std::cout << "Short black castle" << std::endl;
                        }
                        else if (files[strSecond].column == COLUMN::C)
                        {

                            files["d8"].piece = BLACK_ROOK;
                            files["d8"].index = files["a8"].index;
                            files["d8"].column = D;
                            files["d8"].show = true;
                            files["d8"].row = 8;

                            files["a8"].piece = FREE;
                            files["a8"].index = -1;
                            files["a8"].column = A;
                            files["a8"].row = 8;
                            files["a8"].show = false;

                            m_pPieces.at(files["d8"].index).pushConstant.model = transformPiece(
                                {files["d8"].column, files["d8"].row});

                            std::cout << "Long black castle" << std::endl;
                        }
                }
            }

            files[strFirst].column = m_pMove.column_from;
            files[strFirst].row = m_pMove.row_from;
            files[strFirst].index = -1;
            // files[strFirst].show = false;
            files[strFirst].piece = FREE;

            // if (files[strSecond].index != -1)
            m_pPieces.at(files[strSecond].index).pushConstant.model = transformPiece(
                {files[strSecond].column, files[strSecond].row});
            m_pSelectedModel = nullptr;

            m_pBoard.Turn(m_pBoard.Turn() == WHITE ? BLACK : WHITE);
            m_pBoard.addMove(std::string(m_pMove.first, 2) + std::string(m_pMove.second, 2));
            m_pUCI.last_move = "";

            m_pMoveAudioObject.should_play = true;
        }
    }

    // Private functions

    void ChessApp::setupCamera()
    {
        glm::vec2 iResolution = glm::vec2(getWindowDimensions().first, getWindowDimensions().second);
        ChessShaderData camData;
        camData.iResolution = iResolution;

        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        camData.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f),
                                   glm::vec3(0.0f, 0.0f, 0.0f),
                                   glm::vec3(1.0f, 0.0f, 0.0f));

        camData.proj = glm::ortho(-(iResolution.x / 2.0f),
                                  (iResolution.x / 2.0f),
                                  -((iResolution.y / 2.0f)),
                                  (iResolution.y / 2.0f), 0.1f, 1000.0f);
        camData.proj[1][1] *= -1;
        camData.iTime += time;
        camData.lastMove = glm::vec4(
            m_pMove.column_from,
            m_pMove.column_to,
            m_pMove.row_from,
            m_pMove.row_to);

        Epsilon::getContext().Renderer()->UpdateRenderPassUniforms(m_pRenderPass, engine::RENDERPASS_SET, &camData);
    }

    glm::mat4 ChessApp::transformBoard(glm::vec3 t, glm::vec3 s)
    {
        glm::mat4 output = glm::mat4(1.0f);

        output = glm::translate(output, t);
        output = glm::scale(output, s);

        return output;
    }

    const glm::mat4 ChessApp::transformPiece(PieceInfo position)
    {
        glm::mat4 output = glm::mat4(1.0f);

        position.row--;
        if (position.row < 0)
            position.row = 0;
        if (position.row > 8)
            position.row = 8;

        glm::vec2 modPiece = glm::vec2(bottom_left_file_pos().x + ((piece_size() * 2) * position.row),
                                       bottom_left_file_pos().y - ((piece_size() * 2) * (float)position.column));

        output = glm::translate(output, glm::vec3(modPiece, -499.0));
        output = glm::scale(output, glm::vec3(piece_size(), piece_size(), 0.0));

        return output;
    }

    glm::vec2 ChessApp::bottom_left_file_pos()
    {
        return glm::vec2(-(board_size() - piece_size()),
                         board_size() - piece_size());
    };

    float ChessApp::piece_size()
    {
        return (getWindowDimensions().second * 0.5f) * .125f;
    }

    float ChessApp::board_size()
    {
        return (getWindowDimensions().second * 0.5f);
    }

    void ChessApp::setupGeometry()
    {
        // Create a quad for the chess board
        auto quad_data = m_pQuad.data();
        m_pBoardModel.mesh = Epsilon::getContext().ResourceManager()->createMesh({.vertices = quad_data.Vertices,
                                                                                  .indices = quad_data.Indices});

        engine::Ref<engine::Mesh> pieceMesh = Epsilon::getContext().ResourceManager()->createMesh(
            {.vertices = quad_data.Vertices,
             .indices = quad_data.Indices});

        m_pBoardModel.pushConstantRef = Epsilon::getContext().ResourceManager()->createPushConstant(
            {.size = sizeof(PiecePushConstant), .data = &m_pBoardModel.pushConstant});

        unsigned index = 0;
        for (auto &[position, info] : m_pBoard.getFiles())
        {
            if (info.piece == FREE)
                continue;

            Model &ref = m_pPieces[index];
            info.index = index;
            ref.pushConstantRef = Epsilon::getContext().ResourceManager()->createPushConstant(
                {.size = sizeof(PiecePushConstant), .data = &ref.pushConstant});
            ref.mesh = pieceMesh;
            ref.pushConstant.model = transformPiece({info.column, info.row});
            index++;
        }
    }

    void ChessApp::setupTextures()
    {
        // create the material for the board

        using en = engine::UniformBindingType;
        {
            engine::MaterialInfo material = {
                .bindingInfo = {{.size = sizeof(ChessShaderData), .type = en::UNIFORM_BUFFER, .binding = 0}}};

            m_pBoardModel.material = Epsilon::getContext().ResourceManager()->createMaterial(material, m_pRenderPass);
        }
        // create the material for the pieces
        engine::Ref<engine::Material> pieceMaterial;
        {
            int w, h, nc;
            unsigned char *pixels = framework::load_image_from_file("./assets/images/pieces.png",
                                                                    &w,
                                                                    &h,
                                                                    &nc);
            engine::TextureCreationInfo texInfo = engine::TextureBuilder()
                                                      .width(w)
                                                      .height(h)
                                                      .numChannels(nc)
                                                      .pixels(pixels);

            engine::MaterialInfo material = {
                .bindingInfo = {{.size = sizeof(ChessShaderData), .type = en::UNIFORM_BUFFER, .binding = 0},
                                {.size = 0, .type = en::TEXTURE_IMAGE_COMBINED_SAMPLER, .offset = 0, .binding = 1, .textureInfo = texInfo}}};

            pieceMaterial = Epsilon::getContext().ResourceManager()->createMaterial(material, m_pRenderPass);
            framework::free_image_data(pixels);
        }
        for (auto &ref : m_pPieces)
        {
            ref.material = pieceMaterial;
        }
    }

    void ChessApp::setupRenderPass()
    {

        using namespace engine;
        // Load the shader that draws the board
        auto boardVertexCode = utils::readFile("./assets/shaders/chess/board-vertex.spv");
        auto boardFragmentCode = utils::readFile("./assets/shaders/chess/board-fragment.spv");

        ShaderInfo boardShaderInfo = {
            .stages = {
                {.entryPoint = "main", .shaderCode = boardVertexCode, .stage = VERTEX},
                {.entryPoint = "main", .shaderCode = boardFragmentCode, .stage = FRAGMENT}},
            .usedStages = ShaderModuleStage(VERTEX | FRAGMENT)};

        // Load the shader that draws the pieces
        auto pieceVertexCode = utils::readFile("./assets/shaders/chess/piece-vertex.spv");
        auto pieceFragmentCode = utils::readFile("./assets/shaders/chess/piece-fragment.spv");
        ShaderInfo pieceShaderInfo = {
            .stages = {
                {.entryPoint = "main", .shaderCode = pieceVertexCode, .stage = VERTEX},
                {.entryPoint = "main", .shaderCode = pieceFragmentCode, .stage = FRAGMENT}},
            .usedStages = ShaderModuleStage(VERTEX | FRAGMENT)};

        std::vector<VertexDescriptorInfo> vertexInfo = {{XYZ_FLOAT, offsetof(Vertex, position)},
                                                        {XY_FLOAT, offsetof(Vertex, texCoords)},
                                                        {XYZ_FLOAT, offsetof(Vertex, normal)},
                                                        {XYZW_FLOAT, offsetof(Vertex, color)},
                                                        {XYZ_FLOAT, offsetof(Vertex, tangent)},
                                                        {XYZ_FLOAT, offsetof(Vertex, bitangent)}};
        PipelineLayout boardLayout = {
            .shaderInfo = boardShaderInfo,
            .vertexLayout = vertexInfo,
            .cullMode = CullMode::BACK,
            .windingMode = WindingMode::COUNTER_CLOCK_WISE,
            .name = "BoardLayout"};

        PipelineLayout pieceLayout = {
            .shaderInfo = pieceShaderInfo,
            .vertexLayout = vertexInfo,
            .cullMode = CullMode::BACK,
            .windingMode = WindingMode::COUNTER_CLOCK_WISE,
            .name = "PieceLayout"};

        // Configure the default render pass object
        RenderPassInfo renderPassInfo =
            RenderPassFactory()
                .numDescriptors(6)
                .size(sizeof(Vertex))
                .depthAttachment(true)
                .subpasses({})
                .dimensions({.width = 1920, .height = 1080})
                .attachments({{
                                  .format = COLOR_RGBA,
                                  .isDepthAttachment = false,
                                  .isSwapChainAttachment = true,
                              },
                              {.format = DEPTH_F32_STENCIL_8,
                               .isDepthAttachment = true}})
                .pipelineLayout(boardLayout)
                .pipelineLayout(pieceLayout)
                .pushConstant(sizeof(PiecePushConstant))
                .uniformBindings({{.size = sizeof(ChessShaderData), .offset = 0, .binding = 0, .type = UniformBindingType::UNIFORM_BUFFER},
                                  {.size = 0, .offset = 0, .binding = 1, .type = UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER}});

        m_pRenderPass = Epsilon::getContext().ResourceManager()->createDefaultRenderPass(renderPassInfo);
    }
}