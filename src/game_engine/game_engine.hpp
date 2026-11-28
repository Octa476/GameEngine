#pragma once

#include "camera/camera.hpp"
#include "event_manager/event_manager.hpp"
#include "render_context/render_context.hpp"

namespace game {

class GameEngine {
   public:
    bool init(RenderCommandList& commandList);
    void addKeyStates(const bool* keyState);
    void addEvents(const SDL_Event& event);
    void update(float deltaTime, RenderCameraInfo& cameraInfo,
                RenderCommandList& commandList, RenderDrawList& drawList);
    void processResults(RenderCommandList& commandList);

   private:
    Camera m_camera;
    EventManager m_eventManager;

    void handleEvents_(float deltaTime);

    std::vector<glm::vec3> rotationAxes;
    std::vector<float> rotationSpeeds;
    std::vector<float> rotationAngles;

    std::vector<glm::vec3> movementDirections;
    std::vector<float> movementSpeeds;
    std::vector<float> pulsePhases;
};
}  // namespace game