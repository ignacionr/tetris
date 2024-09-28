#pragma once
#include <array>
#include <utility>   // For std::pair

enum class tetrimino_t
{
    none,
    I,
    O,
    T,
    L,
    J,
    S,
    Z,
    end
};

using coord_t = std::pair<unsigned char, unsigned char>;
using four_blocks_t = std::array<coord_t, 4>;
using shape_t = std::pair<four_blocks_t, double>;
constexpr static auto game_height = 26U;
constexpr static auto game_width = 15U;
using board_t = std::array<std::array<tetrimino_t, game_width>, game_height>;