#include <chrono>
#include <functional>
#include <unordered_map>

#include "tetris_controls.hpp"
#include "tetris_game.hpp"
#include "tetris_screen.hpp"
#include "tetris_audio.hpp"

int main() {
    static constexpr char theme_audio[] {"assets/tetris.wav"};
    static constexpr char pop_audio[] {"assets/pop.wav"};

    bool quit{false};
    tetris_game game;
    tetris_controls controls;
    tetris_screen screen;
    tetris_audio audio{theme_audio};
    tetris_audio audio_pop{pop_audio};
    game.on_pop = [&audio_pop](){
        audio_pop.play();
    };
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
    audio.play(1.0f);
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
