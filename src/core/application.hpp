#pragma once

#include <SDL3/SDL.h>

#include "SDL3/SDL_init.h"
#include "game_engine/game_engine.hpp"
#include "platform/window.hpp"
#include "render_context/render_context.hpp"
#include "renderer/renderer.hpp"

namespace engine {

class Application {
   public:
    Application() = default;
    ~Application() = default;

    SDL_AppResult init();
    SDL_AppResult onEvent(const SDL_Event& event);
    SDL_AppResult frame();

   private:
    Window m_window;
    Renderer m_renderer;
    game::GameEngine m_gameEngine;
    RenderContext m_renderContext;

    Uint64 m_lastTick;
    float m_deltaTime;
};
}  // namespace engine
