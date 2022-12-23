//
// Created by solaire on 12/15/22.
//

#include <Epsilon.hpp>
#include <beacon/beacon.hpp>
#include "engine/renderer/drawables/primitives/quad.hpp"
#include "UCI.hpp"

namespace ChessApp {
    class ChessApp : public Epsilon::Epsilon {

        struct PiecePushConstant {
            alignas(16) glm::mat4 model;
            alignas(16) uint32_t piece;
        };

        enum PIECE {
            WHITE_KING = 0,
            WHITE_QUEEN,
            WHITE_BISHOP,
            WHITE_KNIGHT,
            WHITE_ROOK,
            WHITE_PAWN,
            BLACK_KING,
            BLACK_QUEEN,
            BLACK_BISHOP,
            BLACK_KNIGHT,
            BLACK_ROOK,
            BLACK_PAWN,

        };

        enum COLUMN {
            A = 0, B, C, D, E, F, G, H
        };

        struct PieceInfo {
            COLUMN column;
            uint32_t row;
            PIECE piece;
        };

        struct Model {
            Model() {}

            Model(PieceInfo i) : info(i) {}

            engine::Ref<engine::Mesh> mesh;
            engine::Ref<engine::Texture> texture;
            engine::Ref<engine::UniformBindings> bindings;
            engine::Ref<engine::Material> material;
            engine::Ref<engine::PushConstant> pushConstantRef;
            PiecePushConstant pushConstant;
            PieceInfo info;
        };


        engine::Quad m_pQuad = {};

        std::list<Model> m_pPieces;

        Model board;

        std::thread m_pEngineThread;

        UCI m_pUCI;

    public:
        explicit ChessApp(const std::string &appName) : Epsilon::Epsilon(appName) {}

        void onCreate() override;


        void onReady() override;

        void onRender() override;

        void onExit() override {}

    private:
        void setupCamera();

        glm::mat4 transformBoard(glm::vec3 t, glm::vec3 s);

        const glm::mat4 transformPiece(PieceInfo position);

        glm::vec2 bottom_left_file_pos();

        float piece_size();

        float board_size();
    };
}