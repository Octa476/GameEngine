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

    if (event.window.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
        LOG_TRACE("Window closed requested");
        return SDL_APP_SUCCESS;
    }

    if (event.wheel.type == SDL_EVENT_MOUSE_WHEEL) {
        if (event.wheel.y > 0.0f) {
            m_camera.zoomIn(event.wheel.y);
        } else {
            m_camera.zoomOut(event.wheel.y);
        }
    }

    if (event.motion.type == SDL_EVENT_MOUSE_MOTION) {
       if (event.motion.yrel) {
           if ((event.motion.yrel < 10.0f) || (event.motion.yrel > -10.0f))
               m_camera.rotatePitch(-event.motion.yrel);
       }

       if (event.motion.xrel) {
           m_camera.rotateYaw(event.motion.xrel);
       }
    }   

    return SDL_APP_CONTINUE;
}

SDL_AppResult engine::Application::frame() {
    Uint64 currentTick = SDL_GetTicks();
    m_deltaTime = currentTick - m_lastTick;
    m_lastTick = currentTick;
    checkStates_();

    if (m_renderer.beginFrame(m_window.handle(), m_camera)) {
        // draw
        if (!m_renderer.endFrame())
            return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

void engine::Application::checkStates_() {
    const bool *key_states = SDL_GetKeyboardState(NULL);

    if (key_states[SDL_SCANCODE_W]) {
        m_camera.translateFront(static_cast<float>(m_deltaTime) / 1000);
    }
    if (key_states[SDL_SCANCODE_S]) {
        m_camera.translateBack(static_cast<float>(m_deltaTime) / 1000);
    }
    if (key_states[SDL_SCANCODE_E]) {
        m_camera.translateUp(static_cast<float>(m_deltaTime) / 1000);
    }
    if (key_states[SDL_SCANCODE_Q]) {
        m_camera.translateDown(static_cast<float>(m_deltaTime) / 1000);
    }
    if (key_states[SDL_SCANCODE_A]) {
        m_camera.translateLeft(static_cast<float>(m_deltaTime) / 1000);
    }
    if (key_states[SDL_SCANCODE_D]) {
        m_camera.translateRight(static_cast<float>(m_deltaTime) / 1000);
    }

}
