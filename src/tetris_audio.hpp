#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

struct tetris_audio {
    tetris_audio(const char *audio_file) {
        // Load WAV file
        if (SDL_LoadWAV(audio_file, &wav_spec, &wav_buffer, &wav_length) == NULL) {
            fprintf(stderr, "Could not open %s: %s\n", audio_file, SDL_GetError());
            return;
        }
        base_frequency = wav_spec.freq;
    };
    ~tetris_audio() {
        if (device_id != 0) SDL_CloseAudioDevice(device_id);
        if (wav_buffer) SDL_FreeWAV(wav_buffer);
    }

    void play(float speed_factor = 1.0f) {
        if (device_id != 0) SDL_CloseAudioDevice(device_id);

        // Modify the frequency for speed adjustment
        wav_spec.freq = (int)(base_frequency * speed_factor);  // Adjust playback speed by altering the frequency
        device_id = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);
        if (device_id == 0) {
            fprintf(stderr, "Failed to open audio device: %s\n", SDL_GetError());
            return;
        }

        // Queue the audio buffer
        if (SDL_QueueAudio(device_id, wav_buffer, wav_length) < 0) {
            fprintf(stderr, "Failed to queue audio data: %s\n", SDL_GetError());
            SDL_CloseAudioDevice(device_id);
            SDL_FreeWAV(wav_buffer);
            SDL_Quit();
            return;
        }

        // Start playback
        SDL_PauseAudioDevice(device_id, 0);
    }
    void toggle_pause() {
        paused = !paused;
        SDL_PauseAudioDevice(device_id, paused);
    }

    SDL_AudioSpec wav_spec;
    Uint32 wav_length;
    Uint8* wav_buffer{nullptr};
    int base_frequency;
    SDL_AudioDeviceID device_id{0};
    bool paused{false};
};
