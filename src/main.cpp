#include "SDL3/SDL_init.h"
#define SDL_MAIN_USE_CALLBACKS 1

#include "core/application.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_AppResult SDL_AppInit(void **appstate, int, char **) {
    auto *app = new engine::Application();
    *appstate = app;
    return app->init();
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    return static_cast<engine::Application *>(appstate)->onEvent(*event);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    return static_cast<engine::Application *>(appstate)->frame();
}

void SDL_AppQuit(void *appstate, SDL_AppResult) {
    delete static_cast<engine::Application *>(appstate);
}
