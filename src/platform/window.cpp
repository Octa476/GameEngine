#include "window.hpp"

#include "SDL3/SDL_video.h"
#include "core/log.hpp"

bool Window::init() {
    window_ = SDL_CreateWindow(config.window_name, config.width, config.height,
                               config.flags);

    if (!window_) {
        LOG_ERROR("{}", SDL_GetError());
        return false;
    }

    return true;
}

bool Window::isHidden() const {
    return (SDL_GetWindowFlags(window_) & SDL_WINDOW_HIDDEN) != 0;
}

bool Window::showWindow() {
    auto ret = SDL_ShowWindow(window_);

    if (!ret) LOG_ERROR("{}", SDL_GetError());

    return ret;
}

Window::~Window() {
    if (window_) {
        SDL_DestroyWindow(window_);
        LOG_TRACE("Destroyed the window");
    }
}
