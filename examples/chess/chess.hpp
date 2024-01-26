//
// Created by solaire on 12/15/22.
//

#include <Epsilon.hpp>
#include <beacon/beacon.hpp>
#include "core/engine/renderer/drawables/primitives/quad.hpp"
#include "UCI.hpp"
#include "game/common.hpp"
#include "examples/chess/game/board.hpp"
#include <core/framework/audio/WAVFile.h>

namespace ChessApp
{
    class ChessApp : public Epsilon::Epsilon
    {

        struct PiecePushConstant
        {
            alignas(16) glm::mat4 model;
            alignas(16) uint32_t piece;
        };

        struct Model
        {
            Model() {}

            engine::Ref<engine::Mesh> mesh;
            engine::Ref<engine::Texture> texture;
            engine::Ref<engine::UniformBindings> bindings;
            engine::Ref<engine::BindGroup> material;
            engine::Ref<engine::PushConstant> pushConstantRef;
            PiecePushConstant pushConstant;
        };

        struct ChessShaderData
        {
            alignas(4) float iTime = 0.0f;
            alignas(8) glm::vec2 iResolution{};
            alignas(16) glm::mat4 view{};
            alignas(16) glm::mat4 proj{};
            alignas(8) glm::vec4 lastMove{};
        };

        engine::Quad m_pQuad = {};

        std::array<Model, 32> m_pPieces;
        engine::Ref<engine::RenderPass> m_pRenderPass;

        Model m_pBoardModel;
        Model *m_pSelectedModel = nullptr;

        std::thread m_pEngineThread;

        UCI m_pUCI;

        Move m_pMove;

        Board m_pBoard;

        struct AudioObject
        {
            framework::WAVfile audioFile;
            al::OpenALSource source;
            al::OpenALBuffer buffer;
            bool should_play = false;
        };

        AudioObject m_pMoveAudioObject;
        AudioObject m_pTakeAudioObject;

    public:
        explicit ChessApp(const std::string &appName) : Epsilon::Epsilon(appName)
        {

            Epsilon::onCreate = [this]
            { onCreate(); };
            Epsilon::onReady = [this]
            { onReady(); };
            Epsilon::onRender = [this]
            { onRender(); };
            Epsilon::onExit = [this]
            { onExit(); };
        }

        void onCreate();

        void onReady();

        void onRender();

        void onExit()
        {
            al::deleteSource(m_pMoveAudioObject.source);
            al::deleteBuffer(m_pMoveAudioObject.buffer);

            al::deleteSource(m_pTakeAudioObject.source);
            al::deleteBuffer(m_pTakeAudioObject.buffer);
            m_pUCI.Move("quit");

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (m_pEngineThread.joinable())
            {
                m_pEngineThread.join();
            }

            std::thread empty;
            m_pEngineThread.swap(empty);
        }

    private:
        void setupCamera();

        void setupGeometry();

        void setupTextures();

        void setupRenderPass();

        glm::mat4 transformBoard(glm::vec3 t, glm::vec3 s);

        const glm::mat4 transformPiece(PieceInfo position);
        const glm::mat4 updatePiecePosition(PieceInfo position);

        glm::vec2 bottom_left_file_pos();

        float piece_size();

        float board_size();
    };
}