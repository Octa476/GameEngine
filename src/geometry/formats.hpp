#pragma once

#include "SDL3/SDL_init.h"
#include "SDL3/SDL_error.h"
#include <string>

namespace Formats
{
    // Vertex input layout.
    struct Vertex
    {
        float x, y, z; // vec3 position
    };

    // Triangle input layout.
    struct Triangle
    {
        Uint32 i, j, k; // vec3 position
    };

    struct Shape {
        const std::vector<Vertex> vertexes;
        const std::vector<Triangle> triangles;
        const std::string name;
        const std::string description;
    };
}