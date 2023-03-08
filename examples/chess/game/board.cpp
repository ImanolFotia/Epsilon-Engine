//
// Created by solaire on 12/25/22.
//

#include "board.hpp"

namespace ChessApp {

    Board::Board(std::string arrangement) {
        m_pArrangement = arrangement;
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

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                m_pFiles["abcdefgh"[j] + std::to_string(i+1)] = {.column = A, .row = 1, .piece = FREE};
            }
        }


        for (auto &info: initialPiecesInfo) {
            m_pFiles["abcdefgh"[info.column] + std::to_string(info.row)] = info;
        }
    }

    void Board::Move(struct Move move) {

    }

    Board::MoveList Board::getPawnMoves(const Player &, const std::string &) {
        return {};
    }

    Board::MoveList Board::getRook(const Player &, const std::string &) {
        return {};
    }

    Board::MoveList Board::getKnight(const Player &, const std::string &) {
        return {};
    }

    Board::MoveList Board::getBishop(const Player &, const std::string &) {
        return {};
    }

    Board::MoveList Board::getQueenMoves(const Player &, const std::string &) {
        return {};
    }

    Board::MoveList Board::getKingMoves(const Player &, const std::string &) {
        return {};
    }

    bool Board::canEnPassant() {
        return false;
    }

    bool Board::canShortCastle() {
        return false;
    }

    bool Board::canLongCastle() {
        return false;
    }

    bool Board::canCapture() {
        return false;
    }

    void Board::isCheck() {

    }


} // ChessApp