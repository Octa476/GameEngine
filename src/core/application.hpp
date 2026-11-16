#pragma once

#include <SDL3/SDL.h>

#include "SDL3/SDL_init.h"
#include "platform/window.hpp"
#include "renderer/renderer.hpp"

namespace engine {

class Application {
  public:
    Application() = default;
    ~Application() = default;

    SDL_AppResult init();
    SDL_AppResult onEvent(const SDL_Event &event);
    SDL_AppResult frame();

  private:
    Window m_window;
    Renderer m_renderer;
};
} // namespace engine
