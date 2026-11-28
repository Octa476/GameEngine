#include "game_engine.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>

#include "core/log.hpp"
#include "geometry/3D_objects.hpp"

namespace game {
bool GameEngine::init(RenderCommandList& commandList) {
    std::vector<MeshProperties>& createMeshList = commandList.createMeshList;
    createMeshList.push_back(MeshProperties{.shape = Geometry::cube});

    std::vector<GraphicsProperties>& createGraphicsList =
        commandList.createGraphicsList;
    createGraphicsList.push_back(GraphicsProperties{
        .type = GraphicsType::Triangles, .name = "Triangle Graphics"});

    return true;
}

void GameEngine::addKeyStates(const bool* keyState) {
    m_eventManager.addKeyStates(keyState);
}

void GameEngine::addEvents(const SDL_Event& event) {
    m_eventManager.addEvent(event);
}

void GameEngine::update(float deltaTime, RenderCameraInfo& cameraInfo,
                        RenderCommandList& commandList,
                        RenderDrawList& drawList) {
    // Handle events.
    handleEvents_(deltaTime);

    // Update cameraInfo.
    cameraInfo.viewMatrix = m_camera.getViewMatrix();
    cameraInfo.projectionMatrix = m_camera.getProjectionMatrix();

    // Update commandList.

    // Update drawList.
    // Generate cubes only once.
    if (drawList.drawList.empty()) {
        std::mt19937 rng(42);

        std::uniform_real_distribution<float> dist(-400.0f, 400.0f);
        std::uniform_real_distribution<float> axisDist(-1.0f, 1.0f);
        std::uniform_real_distribution<float> speedDist(0.3f, 2.0f);

        for (int i = 0; i < 100000; ++i) {
            glm::vec3 position(dist(rng), dist(rng), dist(rng));

            glm::vec3 axis(axisDist(rng), axisDist(rng), axisDist(rng));

            if (glm::length(axis) < 0.001f) axis = glm::vec3(0.0f, 1.0f, 0.0f);

            axis = glm::normalize(axis);

            rotationAxes.push_back(axis);
            rotationSpeeds.push_back(speedDist(rng));
            rotationAngles.push_back(0.0f);
            movementDirections.push_back(glm::normalize(
                glm::vec3(axisDist(rng), axisDist(rng), axisDist(rng))));

            movementSpeeds.push_back(speedDist(rng));
            pulsePhases.push_back(axisDist(rng) * glm::pi<float>());

            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);

            drawList.drawList.push_back(Object{
                .model = model,
                .color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
                .meshId = commandList.resultMeshList[0].mesh_id,
                .graphicsId = commandList.resultGrapicsList[0].graphics_id});
        }
    }

    // Update cube.
    for (size_t i = 0; i < drawList.drawList.size(); ++i) {
        auto& object = drawList.drawList[i];

        // Update animation parameters.
        rotationAngles[i] += rotationSpeeds[i] * deltaTime;
        pulsePhases[i] += movementSpeeds[i] * deltaTime;

        // Original position.
        glm::vec3 position(object.model[3]);

        // Oscillating translation.
        position += movementDirections[i] * (2.0f * std::sin(pulsePhases[i]));

        // Pulsating scale.
        float scale = 1.0f + 2.0f * std::sin(pulsePhases[i] * 1.5f);

        // Rebuild model matrix: Translation * Rotation * Scale.
        glm::mat4 model(1.0f);

        model = glm::translate(model, position);
        model = glm::rotate(model, rotationAngles[i], rotationAxes[i]);
        model = glm::scale(model, glm::vec3(scale));

        object.model = model;
    }
}

void GameEngine::processResults(RenderCommandList& commandList) {
    std::vector<MeshHandler>& resultMeshList = commandList.resultMeshList;
    std::vector<GraphicsHandler>& resultGraphicsList =
        commandList.resultGrapicsList;
}

void GameEngine::handleEvents_(float deltaTime) {
    // Consume events from the eventManger.
    while (m_eventManager.hasEvents()) {
        SDL_Event event = m_eventManager.consumeEvent();

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
    }

    const bool* key_states = m_eventManager.getKeyStates();

    if (key_states[SDL_SCANCODE_W]) {
        m_camera.translateFront(static_cast<float>(deltaTime));
    }
    if (key_states[SDL_SCANCODE_S]) {
        m_camera.translateBack(static_cast<float>(deltaTime));
    }
    if (key_states[SDL_SCANCODE_E]) {
        m_camera.translateUp(static_cast<float>(deltaTime));
    }
    if (key_states[SDL_SCANCODE_Q]) {
        m_camera.translateDown(static_cast<float>(deltaTime));
    }
    if (key_states[SDL_SCANCODE_A]) {
        m_camera.translateLeft(static_cast<float>(deltaTime));
    }
    if (key_states[SDL_SCANCODE_D]) {
        m_camera.translateRight(static_cast<float>(deltaTime));
    }
}
}  // namespace game