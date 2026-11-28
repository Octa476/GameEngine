#pragma once

#include <queue>

#include "SDL3/SDL_init.h"

namespace game {
class EventManager {
   public:
    EventManager();
    bool addKeyStates(const bool* key_states);
    const bool* getKeyStates() const;
    bool hasEvents() const;
    SDL_Event consumeEvent();
    bool addEvent(const SDL_Event& event);

   private:
    std::queue<SDL_Event> events_;
    const bool* key_states_;
};
}  // namespace game