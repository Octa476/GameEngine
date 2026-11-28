#pragma once

#include <vector>
#include <cstdint>
#include "geometry/formats.hpp"
#include "game_engine/object/object.hpp"

struct RenderDrawList {
    std::vector<game::Object> drawList;
};

struct MeshProperties {
    uint64_t generation_id;
    const Formats::Shape shape;
};

struct MeshHandler {
    uint64_t generation_id;
    MeshId mesh_id;
};

enum class GraphicsType {
    Triangles,
    Lines,
};

struct GraphicsProperties {
    uint64_t generation_id;
    const GraphicsType type;
    const std::string name;
};

struct GraphicsHandler {
    uint64_t generation_id;
    GraphicsId graphics_id;
};


struct RenderCommandList {
    std::vector<MeshProperties> createMeshList;
    std::vector<MeshHandler> resultMeshList;
    std::vector<MeshHandler> deleteMeshList;

    std::vector<GraphicsProperties> createGraphicsList;
    std::vector<GraphicsHandler> resultGrapicsList;
    std::vector<GraphicsHandler> deleteGrapicsList;
};

struct RenderCameraInfo {
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};


class RenderContext {
    public:
        bool init();
        RenderDrawList &getRenderDrawList();

        RenderCameraInfo &getRenderCameraInfo();

        RenderCommandList &getRenderCommandList();
    private:
        RenderDrawList renderDrawList;
        RenderCommandList renderCommandList;
        RenderCameraInfo renderCameraInfo;
};

