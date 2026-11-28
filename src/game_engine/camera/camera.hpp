#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
   public:
    Camera() = default;
    Camera(glm::vec3 cameraPos, glm::vec3 cameraTarget, glm::vec3 worldUp,
           float fov, float aspect, float near, float far);
    ~Camera();

    glm::mat4 getViewMatrix() const;

    glm::mat4 getProjectionMatrix() const;

    void zoomIn(float scrollOffset);
    void zoomOut(float scrollOffset);

    void translateFront(float moveOffset);
    void translateBack(float moveOffset);

    void translateUp(float moveOffset);
    void translateDown(float moveOffset);

    void translateRight(float moveOffset);
    void translateLeft(float moveOffset);

    void rotatePitch(float moveOffset);
    void rotateYaw(float moveOffset);

   private:
    // View matrix prerequisites.
    glm::vec3 cameraPos_{0.0f, 2.0f, 6.0f};
    glm::vec3 cameraFront_{0.0f, -2.0f, -6.0f};
    glm::vec3 worldUp_{0.0f, 1.0f, 0.0f};

    // Projection matrix prerequisites.
    float fov_{45.0f};
    float aspect_{1280.0f / 720.0f};
    float near_{0.1f};
    float far_{100.0f};

    static constexpr float fovMin_{1.0f};
    static constexpr float fovMax_{90.0f};

    float sensitivityZoomIn_{1.7f};
    float sensitivityZoomOut_{1.7f};

    float sensitivityMoveFront{40.7f};
    float sensitivityMoveBack{40.7f};

    float sensitivityMoveUp{40.7f};
    float sensitivityMoveDown{40.7f};

    float sensitivityMoveRight{40.7f};
    float sensitivityMoveLeft{40.7f};

    float sensitivityPitch{0.2f};
    float sensitivityYaw{0.2f};
};
