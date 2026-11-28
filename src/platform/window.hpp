#pragma once

#include <SDL3/SDL.h>

#include <core/log.hpp>
#include <cstdint>

#include "SDL3/SDL_video.h"

class Window {
   public:
    Window() = default;
    ~Window();

    bool init();
    bool isHidden() const;
    bool showWindow();

    SDL_Window* handle() const { return window_; }

   private:
    struct DefaultConfig {
        const char* window_name{"GameEngine"};
        uint32_t width{1280};
        uint32_t height{720};
        SDL_WindowFlags flags{SDL_WINDOW_RESIZABLE |
                              SDL_WINDOW_HIGH_PIXEL_DENSITY |
                              SDL_WINDOW_HIDDEN};
    } config;

    SDL_Window* window_;
};
