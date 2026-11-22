#pragma once

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_error.h"

namespace Formats
{
    // Vertex input layout.
    struct Vertex
    {
        float x, y, z;    // vec3 position
    };

    // Triangle input layout.
    struct Triangle
    {
        Uint32 i, j, k; // vec3 position
    };
}