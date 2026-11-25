#pragma once

#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <string>

// #define TINYOBJLOADER_IMPLEMENTATION
// #include "tiny_obj_loader.h"

/*
    This class holds a copy of the GPU buffers allocated for a specific model. 
*/
struct Mesh {
    private:
        SDL_GPUBuffer *vertexBuffer_ {nullptr};
        SDL_GPUBuffer *indexBuffer_ {nullptr};

        Uint64 vertexCount_ {0};
        Uint64 indexCount_ {0};

        std::string name_ {};
        std::string description_ {};

        friend class Renderer;
    
    public:
        explicit Mesh(const std::string &name, 
            SDL_GPUBuffer *vertexBuffer, Uint64 vertexCount, 
            SDL_GPUBuffer *indexBuffer, Uint64 indexCount, 
            const std::string &description="Mesh");
        const std::string &getMeshName() const;
        const std::string &getMeshDescription() const;
};