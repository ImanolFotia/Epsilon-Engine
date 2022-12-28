//
// Created by solaire on 12/25/22.
//

#ifndef EPSILON_PLAYER_HPP
#define EPSILON_PLAYER_HPP

#include <utility>
#include <functional>

#include "common.hpp"

namespace ChessApp {

    class Player {
        COLOR m_pColor;
        std::string m_pName;
        bool m_pHasCastled = false;
        std::vector<PIECE> m_pCaptures;

    public:
        explicit Player(COLOR color, std::string name = "Unknown") : m_pColor(color), m_pName(std::move(name)) {}

        std::function<void(std::string)> Move;
    };
}


#endif //EPSILON_PLAYER_HPP
