#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>

using MeshId = uint64_t;
using GraphicsId = uint64_t;

namespace game {

    struct Object {
        glm::mat4 model {glm::mat4(1.0f)};
        glm::vec4 color {glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)};
        MeshId meshId;
        GraphicsId graphicsId;

        std::string to_string() {
            return "";
        }
    };
}