#pragma once

#include <vector>
#include "tetrimino.hpp"

enum class animation_type {
    row_collapse
};

struct game_animation {
    animation_type type;
    float duration;
    std::vector<coord_t> focus;
    board_t initial_state;
};
