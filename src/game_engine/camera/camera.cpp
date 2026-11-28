#include "camera.hpp"

#include "core/log.hpp"

Camera::Camera(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 worldUp,
               float fov, float aspect, float near, float far)
    : cameraPos_(cameraPos),
      cameraFront_(cameraFront),
      worldUp_(worldUp),
      fov_(fov),
      aspect_(aspect),
      near_(near),
      far_(far) {}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(cameraPos_, cameraPos_ + cameraFront_, worldUp_);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov_), aspect_, near_, far_);
}

void Camera::zoomIn(float scrollOffset) {
    if (fov_ >= fovMin_) {
        fov_ -= sensitivityZoomIn_ * abs(scrollOffset);
    }
}

void Camera::zoomOut(float scrollOffset) {
    if (fov_ <= fovMax_) {
        fov_ += sensitivityZoomOut_ * abs(scrollOffset);
    }
}

void Camera::translateFront(float moveOffset) {
    cameraPos_ += cameraFront_ * moveOffset * sensitivityMoveFront;
}

void Camera::translateBack(float moveOffset) {
    cameraPos_ -= cameraFront_ * moveOffset * sensitivityMoveFront;
}

void Camera::translateUp(float moveOffset) {
    cameraPos_ += worldUp_ * moveOffset * sensitivityMoveUp;
}

void Camera::translateDown(float moveOffset) {
    cameraPos_ -= worldUp_ * moveOffset * sensitivityMoveDown;
}

void Camera::translateRight(float moveOffset) {
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront_, worldUp_));
    cameraPos_ += cameraRight * moveOffset * sensitivityMoveRight;
}

void Camera::translateLeft(float moveOffset) {
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront_, worldUp_));
    cameraPos_ -= cameraRight * moveOffset * sensitivityMoveRight;
}

void Camera::rotatePitch(float moveOffset) {
    float angle = moveOffset * sensitivityPitch;
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront_, worldUp_));
    glm::mat4 rotateMatrix =
        glm::rotate(glm::mat4(1.0f), glm::radians(angle), cameraRight);
    cameraFront_ =
        glm::normalize(glm::vec3(rotateMatrix * glm::vec4(cameraFront_, 1.0f)));
}

void Camera::rotateYaw(float moveOffset) {
    float angle = moveOffset * sensitivityYaw;
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront_, worldUp_));
    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraFront_, cameraRight));
    glm::mat4 rotateMatrix =
        glm::rotate(glm::mat4(1.0f), glm::radians(angle), cameraUp);
    cameraFront_ =
        glm::normalize(glm::vec3(rotateMatrix * glm::vec4(cameraFront_, 1.0f)));
}

Camera::~Camera() {}