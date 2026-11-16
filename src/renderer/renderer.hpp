#pragma once

#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>

class Renderer {
  public:
    Renderer() = default;
    ~Renderer();

    bool init(SDL_Window *window);
    bool beginFrame(SDL_Window *window);
    bool endFrame();

  private:
    SDL_GPUDevice *device_{nullptr};
    SDL_GPUCommandBuffer *commandBuffer_{nullptr};
    SDL_GPURenderPass *renderPass_{nullptr};
};
