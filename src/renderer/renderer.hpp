#pragma once

#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../geometry/formats.hpp"
#include "../camera/camera.hpp"
#include "mesh.hpp"

class Renderer
{
public:
  Renderer() = default;
  ~Renderer();

  bool init(SDL_Window *window);
  bool beginFrame(SDL_Window *window, const Camera &camera);
  bool endFrame();

private:
  static constexpr std::string shader_dir{"src/shaders/"};

  SDL_GPUDevice *device_{nullptr};
  SDL_GPUCommandBuffer *commandBuffer_{nullptr};
  SDL_GPURenderPass *renderPass_{nullptr};
  SDL_GPUGraphicsPipeline *graphicsPipeline_{nullptr};
  std::unordered_map<std::string, std::shared_ptr<Mesh>> mesh_map_;

  bool createGraphicsPipeline_(SDL_Window *window);
  SDL_GPUBuffer *createVertexBuffer_(const std::vector<Formats::Vertex> &vertices);
  SDL_GPUBuffer *createIndexBuffer_(const std::vector<Formats::Triangle> &triangles);

  struct UniformBuffer
  {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec4 color;
  };
};
