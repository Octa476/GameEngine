#include "application.hpp"
#include "SDL3/SDL_init.h"
#include "core/log.hpp"

SDL_AppResult engine::Application::init() {
    engine::Log::init();
    engine::Log::addSink<engine::ConsoleSink>();

    if (!m_window.init())
        return SDL_APP_FAILURE;

    if (m_window.isHidden()) {
        auto ret = m_window.showWindow();

        if (!ret)
            return SDL_APP_FAILURE;
    }

    if (!m_renderContext.init()) {
        return SDL_APP_FAILURE;
    }

    // Init the engine and then create a commandList for the renderer.
    if (!m_gameEngine.init(m_renderContext.getRenderCommandList())) {
        return SDL_APP_FAILURE;
    }

    LOG_INFO("Game Engine initialization successful!");

    m_gameEngine.addKeyStates(SDL_GetKeyboardState(NULL));

    // Init the renderer and then give it the commandList for execution.
    if (!m_renderer.init(m_window.handle(), m_renderContext.getRenderCommandList()))
        return SDL_APP_FAILURE;
    
    LOG_INFO("Rendered initialization successful!");

    // Update the engine with the results created after the execution of commands.
    m_gameEngine.processResults(m_renderContext.getRenderCommandList());

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

    m_gameEngine.addEvents(event);

    return SDL_APP_CONTINUE;
}

SDL_AppResult engine::Application::frame() {
    Uint64 currentTick = SDL_GetTicks();
    m_deltaTime = (currentTick - m_lastTick) / 1000.0f;
    m_lastTick = currentTick;

    // Update the game state, camera info, commands given for execution to
    // the renderer and the renderDrawList.
    m_gameEngine.update(m_deltaTime, m_renderContext.getRenderCameraInfo(), 
                                    m_renderContext.getRenderCommandList(), 
                                    m_renderContext.getRenderDrawList());

    // Execute render commands, e.g create a new mesh or graphics pipeline.
    m_renderer.executeCommands(m_window.handle(), m_renderContext.getRenderCommandList());

    // Update the engine with the results created after the execution of commands.
    m_gameEngine.processResults(m_renderContext.getRenderCommandList());

    // Render shit.
    if (m_renderer.beginFrame(m_window.handle())) {
        m_renderer.render(m_window.handle(), m_renderContext.getRenderCameraInfo(), 
                        m_renderContext.getRenderDrawList());
        if (!m_renderer.endFrame())
            return SDL_APP_FAILURE;
    }
    
    return SDL_APP_CONTINUE;
}
