#include "event_manager.hpp"

game::EventManager::EventManager() {}

bool game::EventManager::addKeyStates(const bool *key_states) {
    key_states_ = key_states;
    return true;
}

const bool *game::EventManager::getKeyStates() const {
    return key_states_;
}

bool game::EventManager::hasEvents() const {
    return !events_.empty();
}

SDL_Event game::EventManager::consumeEvent() {
    SDL_Event event = events_.front();
    events_.pop();
    return event;
}

bool game::EventManager::addEvent(const SDL_Event &event) {
    events_.push(event);
    return true;
}