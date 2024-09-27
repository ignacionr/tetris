#pragma once
#include <array>

enum class tetrimino_t {
    none,
    I,
    O,
    T,
    L,
    J,
    S,
    Z
};

struct tetris_game {
    tetris_game (){
        board[0][0] = 1;
        board[1][1] = 2;
        board[2][2] = 3;
    }

    constexpr static size_t height = 22;
    constexpr static size_t width = 15;

    std::array<std::array<char, width>, height> board{};
    void step() {
        for (int row = height - 1; row > 0; --row) {
            board[row] = board[row-1];
        }
        board[0] = {};
    }
};
