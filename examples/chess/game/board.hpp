//
// Created by solaire on 12/25/22.
//

#ifndef EPSILON_BOARD_HPP
#define EPSILON_BOARD_HPP

#include <utility>

#include "common.hpp"
#include "player.hpp"

namespace ChessApp
{

    class Board
    {
        using MoveList = std::list<std::string>;

        std::string m_pArrangement;

        std::vector<std::string> m_pMoves = {};
        std::unordered_map<std::string, PieceInfo> m_pFiles;

        COLOR m_pTurn = WHITE;
        bool m_pBlackCastled = false;
        bool m_pWhiteCastled = false;

    public:
        explicit Board(std::string arrangement = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

        void Move(struct Move);

        MoveList getPawnMoves(const Player &, const std::string &);
        MoveList getRook(const Player &, const std::string &);
        MoveList getKnight(const Player &, const std::string &);
        MoveList getBishop(const Player &, const std::string &);
        MoveList getQueenMoves(const Player &, const std::string &);
        MoveList getKingMoves(const Player &, const std::string &);

        bool canEnPassant();
        bool canShortCastle();
        bool canLongCastle();
        bool canCapture();

        bool BlackCastled() { return m_pBlackCastled; }
        bool WhiteCastled() { return m_pWhiteCastled; }

        void BlackCastled(bool x) { m_pBlackCastled = x; }
        void WhiteCastled(bool x) { m_pWhiteCastled = x; }

        void isCheck();

        std::unordered_map<std::string, PieceInfo> &getFiles()
        {
            return m_pFiles;
        }

        const COLOR Turn()
        {
            return m_pTurn;
        }

        void Turn(COLOR turn)
        {
            m_pTurn = turn;
        }

        void addMove(std::string move)
        {
            m_pMoves.push_back(move);
        }

        auto getPlayedMoves()
        {
            return m_pMoves;
        }
    };

} // ChessApp

#endif // EPSILON_BOARD_HPP
