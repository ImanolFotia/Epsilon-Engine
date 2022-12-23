//
// Created by solaire on 12/22/22.
//

#include "chess.hpp"

namespace ChessApp {

    void ChessApp::onCreate() {

        using namespace engine;

        //Create a quad for the chess board
        auto quad_data = m_pQuad.data();
        board.mesh = m_pContext.ResourceManager()->createMesh({.vertices = quad_data.Vertices,
                                                                      .indices = quad_data.Indices});

        engine::Ref<engine::Mesh> pieceMesh = m_pContext.ResourceManager()->createMesh(
                {.vertices = quad_data.Vertices,
                        .indices = quad_data.Indices});
        //Load the shader that draws the board
        auto boardVertexCode = utils::readFile("assets/shaders/board-vertex.spv");
        auto boardFragmentCode = utils::readFile("assets/shaders/board-fragment.spv");


        ShaderInfo boardShaderInfo = {
                .stages = {
                        {.entryPoint = "main", .shaderCode = boardVertexCode, .stage = VERTEX},
                        {.entryPoint = "main", .shaderCode = boardFragmentCode, .stage = FRAGMENT}},
                .usedStages = ShaderModuleStage(VERTEX | FRAGMENT)};

        //Load the shader that draws the pieces
        auto pieceVertexCode = utils::readFile("assets/shaders/piece-vertex.spv");
        auto pieceFragmentCode = utils::readFile("assets/shaders/piece-fragment.spv");
        ShaderInfo pieceShaderInfo = {
                .stages = {
                        {.entryPoint = "main", .shaderCode = pieceVertexCode, .stage = VERTEX},
                        {.entryPoint = "main", .shaderCode = pieceFragmentCode, .stage = FRAGMENT}},
                .usedStages = ShaderModuleStage(VERTEX | FRAGMENT)};

        //Configure the default render pass object
        RenderPassInfo renderPassInfo =
                RenderPassFactory()
                        .numDescriptors(6)
                        .size(sizeof(Vertex))
                        .depthAttachment(true)
                        .subpasses({})
                        .vertexLayout({{XYZ_FLOAT,  offsetof(Vertex, position)},
                                       {XY_FLOAT,   offsetof(Vertex, texCoords)},
                                       {XYZ_FLOAT,  offsetof(Vertex, normal)},
                                       {XYZW_FLOAT, offsetof(Vertex, color)},
                                       {XYZ_FLOAT,  offsetof(Vertex, tangent)},
                                       {XYZ_FLOAT,  offsetof(Vertex, bitangent)}})
                        .attachments(
                                {
                                        {
                                                .format = COLOR_RGBA,
                                                .isDepthAttachment = false,
                                                .isSwapChainAttachment = true
                                        },
                                        {
                                                .format = DEPTH_F32_STENCIL_8,
                                                .isDepthAttachment = true
                                        }
                                })
                        .pipelineLayout({.shaderInfo = boardShaderInfo})
                        .pipelineLayout({.shaderInfo = pieceShaderInfo})
                        .pushConstant(sizeof(PiecePushConstant))
                        .bufferInfo({.size = sizeof(ShaderData), .offset = 0});

        renderPassRef = m_pContext.ResourceManager()->createDefaultRenderPass(renderPassInfo);

        //create the material for the board
        {
            MaterialInfo material = {
                    .bindingInfo = {
                            .size = sizeof(ShaderData),
                            .offset = 0
                    }
            };

            board.material = m_pContext.ResourceManager()->createMaterial(material, renderPassRef);
        }
//create the material for the pieces
        engine::Ref<engine::Material> pieceMaterial;
        {
            MaterialInfo material = {
                    .bindingInfo = {
                            .size = sizeof(ShaderData),
                            .offset = 0
                    }
            };
            int w, h, nc;
            unsigned char *pixels = framework::load_image_from_file("assets/images/pieces.png",
                                                                    &w,
                                                                    &h,
                                                                    &nc);
            engine::TextureInfo texInfo = engine::TextureBuilder()
                    .width(w)
                    .height(h)
                    .numChannels(nc)
                    .pixels(pixels);

            material.textures.push_back(texInfo);
            pieceMaterial = m_pContext.ResourceManager()->createMaterial(material, renderPassRef);
            framework::free_image_data(pixels);
        }

        board.pushConstantRef = m_pContext.ResourceManager()->createPushConstant(
                {.size = sizeof(PiecePushConstant), .data = &board.pushConstant});

        std::list<PieceInfo> initialPiecesInfo = {
                {.column = A, .row = 1, .piece = WHITE_ROOK},
                {.column = B, .row = 1, .piece = WHITE_KNIGHT},
                {.column = C, .row = 1, .piece = WHITE_BISHOP},
                {.column = D, .row = 1, .piece = WHITE_QUEEN},
                {.column = E, .row = 1, .piece = WHITE_KING},
                {.column = F, .row = 1, .piece = WHITE_BISHOP},
                {.column = G, .row = 1, .piece = WHITE_KNIGHT},
                {.column = H, .row = 1, .piece = WHITE_ROOK},
                //white pawns
                {.column = A, .row = 2, .piece = WHITE_PAWN},
                {.column = B, .row = 2, .piece = WHITE_PAWN},
                {.column = C, .row = 2, .piece = WHITE_PAWN},
                {.column = D, .row = 2, .piece = WHITE_PAWN},
                {.column = E, .row = 2, .piece = WHITE_PAWN},
                {.column = F, .row = 2, .piece = WHITE_PAWN},
                {.column = G, .row = 2, .piece = WHITE_PAWN},
                {.column = H, .row = 2, .piece = WHITE_PAWN},

                //black pawns
                {.column = A, .row = 7, .piece = BLACK_PAWN},
                {.column = B, .row = 7, .piece = BLACK_PAWN},
                {.column = C, .row = 7, .piece = BLACK_PAWN},
                {.column = D, .row = 7, .piece = BLACK_PAWN},
                {.column = E, .row = 7, .piece = BLACK_PAWN},
                {.column = F, .row = 7, .piece = BLACK_PAWN},
                {.column = G, .row = 7, .piece = BLACK_PAWN},
                {.column = H, .row = 7, .piece = BLACK_PAWN},

                //black pieces

                {.column = A, .row = 8, .piece = BLACK_ROOK},
                {.column = B, .row = 8, .piece = BLACK_KNIGHT},
                {.column = C, .row = 8, .piece = BLACK_BISHOP},
                {.column = D, .row = 8, .piece = BLACK_QUEEN},
                {.column = E, .row = 8, .piece = BLACK_KING},
                {.column = F, .row = 8, .piece = BLACK_BISHOP},
                {.column = G, .row = 8, .piece = BLACK_KNIGHT},
                {.column = H, .row = 8, .piece = BLACK_ROOK},
        };

        for (auto &info: initialPiecesInfo) {
            Model &ref = m_pPieces.emplace_back(info);
            ref.pushConstantRef = m_pContext.ResourceManager()->createPushConstant(
                    {.size = sizeof(PiecePushConstant), .data = &ref.pushConstant});
            ref.mesh = pieceMesh;
            ref.material = pieceMaterial;
            ref.pushConstant.model = transformPiece({ref.info.column, ref.info.row});
        }


    }

    void ChessApp::onReady() {
        m_pEngineThread = std::thread([this]() { m_pUCI.init(); });
        m_pEngineThread.detach();
    }

    void ChessApp::onRender() {
        //Set up the camera data
        setupCamera();
        engine::ObjectData objectData;


        board.pushConstant.model = transformBoard(glm::vec3(0.0, 0.0, -500.0),
                                                  glm::vec3(board_size(), board_size(), 0.0));
        objectData.layout_index = 0;
        objectData.mesh = board.mesh;
        objectData.material = board.material;
        objectData.modelMatrix = board.pushConstant.model;
        objectData.pushConstant = board.pushConstantRef;
        m_pContext.Renderer()->Push(objectData);

        //set up the pieces draw data

        for (Model &piece: m_pPieces) {
            piece.pushConstant.piece = piece.info.piece;

            objectData.layout_index = 1;
            objectData.mesh = piece.mesh;
            objectData.material = piece.material;
            objectData.pushConstant = piece.pushConstantRef;
            m_pContext.Renderer()->Push(objectData);
        }

    }

//Private functions

    void ChessApp::setupCamera() {
        ShaderData camData;
        camData.iResolution = glm::vec2(getWindowDimensions().first, getWindowDimensions().second);
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        camData.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f),
                                   glm::vec3(0.0f, 0.0f, 0.0f),
                                   glm::vec3(1.0f, 0.0f, 0.0f));
        camData.proj = glm::ortho(-(getWindowDimensions().first / 2.0f),
                                  (getWindowDimensions().first / 2.0f),
                                  -((getWindowDimensions().second / 2.0f)),
                                  (getWindowDimensions().second / 2.0f), 0.1f, 1000.0f);
        camData.proj[1][1] *= -1;
        camData.iTime += time;
        PushShaderData(camData);
    }

    glm::mat4 ChessApp::transformBoard(glm::vec3 t, glm::vec3 s) {
        glm::mat4 output = glm::mat4(1.0f);

        output = glm::translate(output, t);
        output = glm::scale(output, s);

        return output;
    }

    const glm::mat4 ChessApp::transformPiece(PieceInfo position) {
        glm::mat4 output = glm::mat4(1.0f);

        position.row--;
        if (position.row < 0) position.row = 0;
        if (position.row > 8) position.row = 8;

        glm::vec2 modPiece = glm::vec2(bottom_left_file_pos().x + ((piece_size() * 2) * position.row),
                                       bottom_left_file_pos().y - ((piece_size() * 2) * (float) position.column));

        output = glm::translate(output, glm::vec3(modPiece, 0.0));
        output = glm::scale(output, glm::vec3(piece_size(), piece_size(), 0.0));

        return output;
    }

    glm::vec2 ChessApp::bottom_left_file_pos() {
        return glm::vec2(-(board_size() - piece_size()),
                         board_size() - piece_size());
    };

    float ChessApp::piece_size() {
        return (getWindowDimensions().second * 0.5f) * .125f;
    }

    float ChessApp::board_size() {
        return (getWindowDimensions().second * 0.5f);
    }
}