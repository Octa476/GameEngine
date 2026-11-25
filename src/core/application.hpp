#pragma once

#include <SDL3/SDL.h>

#include "SDL3/SDL_init.h"
#include "platform/window.hpp"
#include "renderer/renderer.hpp"
#include "camera/camera.hpp"

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
    Camera m_camera;
    Uint64 m_lastTick;
    Uint64 m_deltaTime;

    void checkStates_();
};
} // namespace engine
