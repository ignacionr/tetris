#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <chrono>
#include <filesystem>
#include <functional>
#include <unordered_map>

#include <libgen.h>
#include <limits.h>
#include <limits.h>  // For PATH_MAX
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "tetris_controls.hpp"
#include "tetris_game.hpp"
#include "tetris_screen.hpp"
#include "tetris_audio.hpp"

int main() {
    static constexpr char theme_audio[] {"assets/tetris.wav"};
    static constexpr char pop_audio[] {"assets/pop.wav"};

    char exe_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);

    if (len == -1) {
        perror("readlink");
        return -1;
    }
    exe_path[len] = '\0';  // Null-terminate the string
    std::filesystem::path exeDir = std::filesystem::path(exe_path).parent_path();

    bool quit{false};
    tetris_game game;
    tetris_controls controls;
    tetris_screen screen;
    tetris_audio audio{(exeDir / theme_audio).string().c_str()};
    tetris_audio audio_pop{(exeDir / pop_audio).string().c_str()};
    auto next_step {std::chrono::system_clock::now() + std::chrono::seconds(1)};
    auto pace {std::chrono::milliseconds(1000)};
    auto update_music = [&audio, &pace]{audio.play(0.9f + 300.0f / (4 * pace.count()));};
    update_music();

    game.on_pop = [&audio_pop](){
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
        // audio_pop.play();
    };
    game.on_levelup = [&pace, update_music]() {
        pace -= std::chrono::milliseconds{pace.count()/ 5};
        update_music();
    };

    std::unordered_map<tetris_key, std::function<void()>> receivers {
        {tetris_key::quit, [&quit] { quit = true; }},
        {tetris_key::down, [&game] { game.step(); }},
        {tetris_key::left, [&game] { game.move_horizontal(-1); }},
        {tetris_key::right, [&game] { game.move_horizontal(1); }},
        {tetris_key::rotate_1, [&game] { game.move_rotate(-1); }},
        {tetris_key::rotate_2, [&game] { game.move_rotate(1); }},
        {tetris_key::toggle_music, [&audio] { audio.toggle_pause(); }}
    };
    while (!quit) {
        controls.process_events([&receivers](tetris_key k){
            auto action_pos = receivers.find(k);
            if (action_pos != receivers.end()) {
                action_pos->second();
            }
        });
        if (std::chrono::system_clock::now() > next_step) {
            auto offset = std::chrono::duration_cast<std::chrono::system_clock::duration>(pace * game.step());
            next_step = std::chrono::system_clock::now() + offset;
        }
        screen.render(game, pace);
    }
}
