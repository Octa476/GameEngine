#pragma once

#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <string>

class Renderer {
  public:
    Renderer() = default;
    ~Renderer();

    bool init(SDL_Window *window);
    bool beginFrame(SDL_Window *window);
    bool endFrame();

  private:
    std::string shader_dir {"src/shaders/"};
    SDL_GPUDevice *device_{nullptr};
    SDL_GPUCommandBuffer *commandBuffer_{nullptr};
    SDL_GPURenderPass *renderPass_{nullptr};
    SDL_GPUGraphicsPipeline *graphicsPipeline{nullptr};
    SDL_GPUBuffer *vertexBuffer_ {nullptr};
};
