#pragma once

#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "mesh/mesh.hpp"
#include "render_context/render_context.hpp"
#include "geometry/formats.hpp"
#include "geometry/3D_objects.hpp"

class Renderer
{
public:
  Renderer() = default;
  ~Renderer();

  bool init(SDL_Window *window, RenderCommandList &commandList);
  bool executeCommands(SDL_Window *window, RenderCommandList &commandList);
  bool beginFrame(SDL_Window *window);
  bool render(SDL_Window *window, const RenderCameraInfo &cameraInfo, RenderDrawList &drawList);
  bool endFrame();

private:
  static constexpr std::string shader_dir{"src/shaders/"};

  SDL_GPUDevice *device_{nullptr};
  SDL_GPUCommandBuffer *commandBuffer_{nullptr};
  SDL_GPURenderPass *renderPass_{nullptr};
  SDL_GPUGraphicsPipeline *graphicsPipeline_{nullptr};
  std::unordered_map<MeshId, std::unique_ptr<Mesh>> mesh_map_;
  std::unordered_map<GraphicsId, SDL_GPUGraphicsPipeline *> graphics_pipeline_map_;

  std::optional<GraphicsId> createGraphicsPipeline_(SDL_Window *window , const GraphicsProperties &properties);
  std::optional<MeshId> createMesh_(const MeshProperties &properties);
  SDL_GPUBuffer *createVertexBuffer_(const std::vector<Formats::Vertex> &vertices);
  SDL_GPUBuffer *createIndexBuffer_(const std::vector<Formats::Triangle> &triangles);

  // Used to compute the hash of a name.
  std::hash<std::string> stringHasher_ {};

  struct UniformBuffer
  {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec4 color;
  };
};
