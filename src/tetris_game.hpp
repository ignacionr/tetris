#pragma once
#include <algorithm>
#include <array>
#include <cmath>     // For std::round
#include <functional>
#include <iostream>
#include <random>
#include <ranges>
#include <unordered_map>
#include <utility>   // For std::pair
#include <vector>

enum class tetrimino_t
{
    none,
    I,
    O,
    T,
    L,
    S,
    end
};

using coord_t = std::pair<u_char, u_char>;
using four_blocks_t = std::array<coord_t, 4>;
using shape_t = std::pair<four_blocks_t, double>;
std::unordered_map<tetrimino_t, shape_t> shapes{{{tetrimino_t::I, shape_t{{coord_t{0, 0}, coord_t{1, 0}, coord_t{2, 0}, coord_t{3, 0}}, 1.5}},
                                                       {tetrimino_t::O, shape_t{{coord_t{0, 0}, coord_t{1, 0}, coord_t{0, 1}, coord_t{1, 1}}, 0.5}},
                                                       {tetrimino_t::T, shape_t{{coord_t{0, 0}, coord_t{0, 1}, coord_t{0, 2}, coord_t{1, 1}}, 1.0}},
                                                       {tetrimino_t::L, shape_t{{coord_t{0, 0}, coord_t{1, 0}, coord_t{2, 0}, coord_t{2, 1}}, 1.0}},
                                                       {tetrimino_t::S, shape_t{{coord_t{0, 0}, coord_t{0, 1}, coord_t{1, 1}, coord_t{1, 2}}, 1.0}}
                                                       }};


constexpr int GRID_MIN = 0;
constexpr int GRID_MAX = 3;

// Precompute cosine and sine values for multiples of 90 degrees
constexpr int cos_values[] = {1, 0, -1, 0};
constexpr int sin_values[] = {0, 1, 0, -1};

auto rotate = [](int rotations, const std::pair<int, int> &p, double center)
{
    rotations = rotations % 4; // Ensure rotations is within [0, 3]
    auto [x, y] = p;

    // Translate point to origin (relative to center)
    double x_rel = x - center;
    double y_rel = y - center;

    // Retrieve precomputed cosine and sine values
    int cos_theta = cos_values[rotations];
    int sin_theta = sin_values[rotations];

    // Apply rotation matrix
    double x_rot = x_rel * cos_theta - y_rel * sin_theta;
    double y_rot = x_rel * sin_theta + y_rel * cos_theta;

    // Translate back to original coordinate space
    double x_final = x_rot + center;
    double y_final = y_rot + center;

    // Round and clamp the results
    int x_int = std::clamp(static_cast<int>(std::round(x_final)), GRID_MIN, GRID_MAX);
    int y_int = std::clamp(static_cast<int>(std::round(y_final)), GRID_MIN, GRID_MAX);

    return std::make_pair(x_int, y_int);
};

struct tetris_game
{
    constexpr static size_t height = 26;
    constexpr static size_t width = 15;

    inline static bool is_free(tetrimino_t t) { return t == tetrimino_t::none; }

    static tetrimino_t random_tetrimino()
    {
        // Create a random device and seed the generator
        static std::random_device dev;
        static std::mt19937 gen(dev());

        // Define the range for the usable values
        constexpr int min_value = static_cast<int>(tetrimino_t::I);
        constexpr int max_value = static_cast<int>(tetrimino_t::S);

        // Create a uniform integer distribution within the usable range
        static std::uniform_int_distribution<> distrib(min_value, max_value);

        // Generate the random integer and cast it to tetrimino_t
        return static_cast<tetrimino_t>(distrib(gen));
    }

    static int random_x() { 
        static std::random_device dev;
        static std::mt19937 gen(dev());
        static std::uniform_int_distribution<> distrib(0, width-4);
        return distrib(gen);
    }

    four_blocks_t rotated() const
    {
        auto const &shape_def{shapes.at(current_tetrimino)};
        auto blocks{shape_def.first};
        for (auto &coord : blocks)
        {
            coord = rotate(current_rotation, coord, shape_def.second);
        }
        return blocks;
    }

    // unplot the current tetrimino
    void plot(bool unplot = false)
    {
        auto const &blocks{rotated()};
        for (auto const &coord : blocks)
        {
            board[coord.first + current_y][coord.second + current_x] = unplot ? tetrimino_t::none : current_tetrimino;
        }
    }

    bool test() const
    {
        auto const &blocks{rotated()};
        return std::ranges::all_of(blocks, [this](auto coord) -> bool
                                   {
            auto const y {coord.first + current_y};
            auto const x {coord.second + current_x};
            return (y < height) && (x < width) && is_free(board[y][x]); });
    }

    bool move_horizontal(int offset)
    {
        auto result{true};
        plot(true);
        current_x += offset;
        if (!test())
        {
            current_x -= offset;
        }
        plot();
        return result;
    }

    bool move_rotate(bool clockwise)
    {
        auto result{true};
        plot(true);
        auto old_rotation = current_rotation;
        current_rotation += clockwise ? 1 : -1;
        if (current_rotation < 0)
        {
            current_rotation += 4;
        }
        if (!test())
        {
            current_rotation = old_rotation;
        }
        plot();
        return result;
    }

    void check_full_lines(int from, int to) {
        for (int row {from}; row <= to; ++row) {
            if (std::ranges::none_of(board[row], is_free)) {
                if (on_pop) on_pop();
                // score
                // scroll down
                for (int srow{row}; srow > 0; --srow) {
                    board[srow] = board[srow-1];
                }
            }
        }
    }

    void step()
    {
        if (finished)
            return;

        // determine if the current tetrimino can drop to the next row
        plot(true);
        ++current_y;
        if (!test())
        {
            --current_y;
            plot();
            check_full_lines(current_y, current_y + 4);
            current_tetrimino = random_tetrimino();
            current_x = random_x();
            current_y = 0;
            if (!test()) {
                finished = true; // game over
            }
        }
        else
        {
            plot();
        }
    }

    std::array<std::array<tetrimino_t, width>, height> board{};
    bool finished{false};
    uint current_step{0};
    tetrimino_t current_tetrimino{random_tetrimino()};
    uint current_y{0};
    uint current_x{1};
    char current_rotation{0};

    std::function<void()> on_pop;
};
