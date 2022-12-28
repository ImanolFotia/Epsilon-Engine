//
// Created by solaire on 12/25/22.
//

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <list>

namespace ChessApp {

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
        FREE
    };

    enum COLUMN {
        A = 0, B, C, D, E, F, G, H
    };

    struct PieceInfo {
        COLUMN column;
        uint32_t row;
        PIECE piece;
        bool show = true;
        int8_t index = -1;
    };

    struct Move {
        char first[2];
        char second[2];
        COLUMN column_from;
        COLUMN column_to;
        uint32_t row_from;
        uint32_t row_to;
        bool selected = false;
    };

    enum COLOR {
        WHITE = 0,
        BLACK
    };
}