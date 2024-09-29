#pragma once
#include <SDL2/SDL.h>
#include "tetris_game.hpp"

#define BLOCK_WIDTH 15
#define BLOCK_HEIGHT 15

struct tetris_screen 
{
    tetris_screen() {
        SDL_Init(SDL_INIT_EVERYTHING);
        pWindow = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            800,
            600,
            SDL_WINDOW_RESIZABLE);
        pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
    }
    ~tetris_screen() {
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
    }

    SDL_Rect block_rect(int row, int col) {
        return SDL_Rect {
            col * BLOCK_WIDTH + 100,
            row * BLOCK_HEIGHT,
            BLOCK_WIDTH,
            BLOCK_HEIGHT};
    }

    void render_board(const board_t &board) {
        SDL_Rect rect{20,20,BLOCK_WIDTH,BLOCK_HEIGHT};
        for (int row = 2; row < game_height; ++row) {
            rect.y = row * BLOCK_HEIGHT;
            for (int col = 0; col < game_width; ++col) {
                rect.x = col * BLOCK_WIDTH + 100;
                auto color = colors[static_cast<int>(board[row][col])];
                SDL_SetRenderDrawColor(pRenderer, color[0], color[1], color[2], 255);
                SDL_RenderFillRect(pRenderer, &rect);
            }
        }
    }

    void draw_animations(tetris_game const &game, int progress) {
        // for now, we will only do the first animation
        auto const &animation = game.animations.front();
        if (progress < 80)  {
            // start by drawing the old board
            render_board(animation.initial_state);
            // now do the hilighting
            for (auto [row,col]: animation.focus) {
                auto rect {block_rect(row,col)};
                SDL_SetRenderDrawColor(pRenderer, 255,255,255, progress);
                SDL_RenderFillRect(pRenderer, &rect);
            }
        }
        else {
            // just draw the final state
            render_board(game.board);
        }
    }

    void render(tetris_game const &game, std::chrono::system_clock::duration pace) {
        SDL_SetRenderDrawColor(pRenderer, 0,0,0,255);
        SDL_RenderClear(pRenderer);
        // are animations in place?
        if (!game.animations.empty()) {
            // how old is the current presentation?
            auto const age {std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - game.last_step).count()};
            auto const pace_milliseconds {std::chrono::duration_cast<std::chrono::milliseconds>(pace).count()};
            auto const progress { age * 100 / pace_milliseconds };
            draw_animations(game, progress);
        }
        else {
            render_board(game.board);
        }
        SDL_Rect rect;
        rect.x = 90;
        rect.w = BLOCK_WIDTH * game_width + 20;
        rect.y = game_height * BLOCK_HEIGHT + game_height / 2;
        rect.h = game_height / 3;
        SDL_SetRenderDrawColor(pRenderer, 255,255,255, 255);
        SDL_RenderFillRect(pRenderer, &rect);
        SDL_RenderPresent(pRenderer);
    }

    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    std::array<std::array<unsigned char,3>, 8> colors{{
        {0,0,0},// none,
        {0,255,255},// I,
        {255,255,0},// O,
        {128,0,128},// T,
        {255, 165, 0},// L,
        {0,0,255},// J,
        {0,255,0},// S,
        {255,0,0}// Z
    }};
};
