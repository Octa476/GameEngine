#include "renderer.hpp"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"
#include "core/log.hpp"

bool Renderer::init(SDL_Window *window) {
    device_ = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV |
                                      SDL_GPU_SHADERFORMAT_MSL |
                                      SDL_GPU_SHADERFORMAT_DXIL,
                                  false, NULL);

    if (!device_) {
        LOG_FATAL("{}", SDL_GetError());
        return false;
    }

    LOG_INFO("GPU backend: {}", SDL_GetGPUDeviceDriver(device_));

    if (!SDL_ClaimWindowForGPUDevice(device_, window)) {
        LOG_FATAL("{}", SDL_GetError());
        return false;
    }

    return true;
}

bool Renderer::beginFrame(SDL_Window *window) {
    commandBuffer_ = SDL_AcquireGPUCommandBuffer(device_);

    if (!commandBuffer_) {
        LOG_FATAL("{}", SDL_GetError());
        return false;
    }

    SDL_GPUTexture *swapchainTexture{nullptr};

    SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer_, window,
                                          &swapchainTexture, nullptr, nullptr);

    if (swapchainTexture == NULL) {
        SDL_SubmitGPUCommandBuffer(commandBuffer_);
        commandBuffer_ = nullptr;
        LOG_TRACE("SDL_WaitAndAcquireGPUSwapchainTexture failed: {}",
                  SDL_GetError());

        return false;
    }

    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color = {240 / 255.0f, 240 / 255.0f, 240 / 255.0f,
                                   255 / 255.0f};
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture = swapchainTexture;

    renderPass_ =
        SDL_BeginGPURenderPass(commandBuffer_, &colorTargetInfo, 1, NULL);

    return true;
}

bool Renderer::endFrame() {
    SDL_EndGPURenderPass(renderPass_);
    renderPass_ = nullptr;

    auto ret = SDL_SubmitGPUCommandBuffer(commandBuffer_);
    commandBuffer_ = nullptr;

    if (!ret) {
        LOG_FATAL("SDL_SubmitGPUCommandBuffer failed: {}", SDL_GetError());
        return false;
    }

    return true;
}

Renderer::~Renderer() {
    if (device_) {
        SDL_DestroyGPUDevice(device_);
        LOG_TRACE("Released GPU device");
    }
}
