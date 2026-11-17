#include "application.hpp"
#include "SDL3/SDL_init.h"
#include "core/log.hpp"

SDL_AppResult engine::Application::init() {
    engine::Log::init();
    engine::Log::addSink<engine::ConsoleSink>();

    if (!m_window.init())
        return SDL_APP_FAILURE;

    if (!m_renderer.init(m_window.handle()))
        return SDL_APP_FAILURE;

    if (m_window.isHidden()) {
        auto ret = m_window.showWindow();

        if (!ret)
            return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult engine::Application::onEvent(const SDL_Event &event) {
    if (event.key.key == SDLK_ESCAPE) {
        LOG_TRACE("Window closed requested");
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult engine::Application::frame() {
    if (m_renderer.beginFrame(m_window.handle())) {
        // draw
        if (!m_renderer.endFrame())
            return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}
