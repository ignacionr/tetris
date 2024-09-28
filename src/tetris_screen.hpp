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
    
    void render(tetris_game const &game, std::chrono::system_clock::duration pace) {
        SDL_SetRenderDrawColor(pRenderer, 0,0,0,255);
        SDL_RenderClear(pRenderer);
        SDL_Rect rect{20,20,BLOCK_WIDTH,BLOCK_HEIGHT};
        for (int row = 2; row < game_height; ++row) {
            rect.y = row * BLOCK_HEIGHT;
            for (int col = 0; col < game_width; ++col) {
                rect.x = col * BLOCK_WIDTH + 100;
                auto color = colors[static_cast<int>(game.board[row][col])];
                SDL_SetRenderDrawColor(pRenderer, color[0], color[1], color[2], 255);
                SDL_RenderFillRect(pRenderer, &rect);
            }
        }
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
