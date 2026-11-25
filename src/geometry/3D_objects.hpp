#pragma once

#include <vector>
#include "formats.hpp"
using namespace Formats;

namespace Geometry
{
    namespace Cube
    {
        inline const std::vector<Vertex> cubeVertexes{
            {0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 1.0f},
            {0.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {0.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 0.0f},
        };

        inline const std::vector<Triangle> cubeTriangles{
            {0, 1, 2},
            {1, 3, 2},
            {2, 3, 7},
            {2, 7, 6},
            {1, 7, 3},
            {1, 5, 7},
            {6, 7, 4},
            {7, 5, 4},
            {0, 4, 1},
            {1, 4, 5},
            {2, 6, 4},
            {0, 2, 4},
        };
    };
};