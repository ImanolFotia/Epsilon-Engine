//
// Created by solaire on 12/25/22.
//

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <list>

#include <glm/glm.hpp>
namespace ChessApp
{

    enum PIECE
    {
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

    enum COLUMN
    {
        A = 0,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        ENUM_MAX
    };

    struct PieceInfo
    {
        COLUMN column;
        uint32_t row;
        PIECE piece;
        bool show = true;
        int8_t index = -1;
        glm::vec2 initialPos;
        glm::vec2 finalPos;
        float currentPos;
    };

    struct Move
    {
        char first[2];
        char second[2];
        COLUMN column_from = (COLUMN)0;
        COLUMN column_to = (COLUMN)0;
        uint32_t row_from = 0;
        uint32_t row_to = 0;
        bool selected = false;
    };

    enum COLOR
    {
        WHITE = 0,
        BLACK
    };
}