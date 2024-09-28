#include <chrono>
#include <functional>
#include <unordered_map>

#include "tetris_controls.hpp"
#include "tetris_game.hpp"
#include "tetris_screen.hpp"

int main() {
    bool quit{false};
    tetris_game game;
    tetris_controls controls;
    tetris_screen screen;
    std::unordered_map<tetris_key, std::function<void()>> receivers {
        {tetris_key::quit, [&quit] { quit = true; }},
        {tetris_key::down, [&game] { game.step(); }},
        {tetris_key::left, [&game] { game.move_horizontal(-1); }},
        {tetris_key::right, [&game] { game.move_horizontal(1); }},
        {tetris_key::rotate_1, [&game] { game.move_rotate(-1); }},
        {tetris_key::rotate_2, [&game] { game.move_rotate(1); }}
    };
    auto next_step {std::chrono::system_clock::now() + std::chrono::seconds(1)};
    auto pace {std::chrono::seconds(1)};
    while (!quit) {
        controls.process_events([&receivers](tetris_key k){
            auto action_pos = receivers.find(k);
            if (action_pos != receivers.end()) {
                action_pos->second();
            }
        });
        if (std::chrono::system_clock::now() > next_step) {
            game.step();
            next_step = std::chrono::system_clock::now() + pace;
        }
        screen.render(game);
    }
}
