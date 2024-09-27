#pragma once

#include <unordered_map>
#include <SDL2/SDL.h>

enum class tetris_key {
    none,
    up,
    down,
    left,
    right,
    rotate_1,
    rotate_2,
    quit
};

struct tetris_controls {
    void process_events (auto cb) {
        SDL_Event ev;
        while(SDL_PollEvent(&ev)) {
            if (ev.type == SDL_KEYDOWN) {
                auto keybev = reinterpret_cast<SDL_KeyboardEvent*>(&ev);
                auto pos = key_map.find(keybev->keysym.scancode);
                if (pos != key_map.end()) {
                    cb(pos->second);
                }
            }
        }
    }

    std::unordered_map<SDL_Scancode, tetris_key> key_map {
        {SDL_SCANCODE_UP, tetris_key::up},
        {SDL_SCANCODE_DOWN, tetris_key::down},
        {SDL_SCANCODE_LEFT, tetris_key::left},
        {SDL_SCANCODE_RIGHT, tetris_key::right},
        {SDL_SCANCODE_SPACE, tetris_key::rotate_1},
        {SDL_SCANCODE_Q, tetris_key::quit}
    };
};
