//
// Created by solaire on 12/15/22.
//

#include <Epsilon.hpp>
#include <beacon/beacon.hpp>
#include "engine/renderer/drawables/primitives/quad.hpp"
#include "UCI.hpp"
#include "game/common.hpp"
#include "examples/chess/game/board.hpp"

namespace ChessApp {
    class ChessApp : public Epsilon::Epsilon {

        struct PiecePushConstant {
            alignas(16) glm::mat4 model;
            alignas(16) uint32_t piece;
        };



        struct Model {
            Model() {}

            engine::Ref<engine::Mesh> mesh;
            engine::Ref<engine::Texture> texture;
            engine::Ref<engine::UniformBindings> bindings;
            engine::Ref<engine::Material> material;
            engine::Ref<engine::PushConstant> pushConstantRef;
            PiecePushConstant pushConstant;
        };



        engine::Quad m_pQuad = {};

        std::array<Model, 32> m_pPieces;
        engine::Ref<engine::RenderPass> m_pRenderPass;

        Model m_pBoardModel;
        Model* m_pSelectedModel = nullptr;

        std::thread m_pEngineThread;

        UCI m_pUCI;

        Move m_pMove;

        Board m_pBoard;

    public:
        explicit ChessApp(const std::string &appName) : Epsilon::Epsilon(appName) {}

        void onCreate() override;


        void onReady() override;

        void onRender() override;

        void onExit() override {}

    private:
        void setupCamera();

        void setupGeometry();

        void setupTextures();

        void setupRenderPass();

        glm::mat4 transformBoard(glm::vec3 t, glm::vec3 s);

        const glm::mat4 transformPiece(PieceInfo position);

        glm::vec2 bottom_left_file_pos();

        float piece_size();

        float board_size();
    };
}