#pragma once

#include <cstdint>
#include <vector>

#include "game_engine/object/object.hpp"
#include "geometry/formats.hpp"
/*
    This file contains the types (structs and objects) which enables the
   communication between the renderer and the game engine as subsystems of the
   application system.

    The game engine emits render draws and allocation/deallocation commands to
   the renderer, the renderer executes the given commands and then returns
   handlers for the objects created. The renderer also reads the state of the
   games engines's camera and so much more in the future.
*/

// Keeps the drawList that's gonna be rendered at each frame.
struct RenderDrawList {
    std::vector<game::Object> drawList;
};

// Struct read by the renderer and written by the game engine; it is used to
// create meshes.
struct MeshProperties {
    uint64_t generation_id;
    const Formats::Shape shape;
};

// Struct read by the game engine and written by the rendered after mesh
// creation.
struct MeshHandler {
    uint64_t generation_id;
    MeshId mesh_id;
};

enum class GraphicsType {
    Triangles,
    Lines,
};

// Struct read by the renderer and written by the game engine; it is used to
// create graphics pipelines.
struct GraphicsProperties {
    uint64_t generation_id;
    const GraphicsType type;
    const std::string name;
};

// Struct read by the game engine and written by the rendered after graphics
// pipeline creation.
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

// Class used by the application system to enable the communication between the
// renderer and game engine subsystems.
class RenderContext {
   public:
    bool init();
    RenderDrawList& getRenderDrawList();

    RenderCameraInfo& getRenderCameraInfo();

    RenderCommandList& getRenderCommandList();

   private:
    RenderDrawList renderDrawList;
    RenderCommandList renderCommandList;
    RenderCameraInfo renderCameraInfo;
};
