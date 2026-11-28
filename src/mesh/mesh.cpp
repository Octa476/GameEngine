#include "mesh.hpp"

#include "core/log.hpp"

Mesh::Mesh(const std::string& name, SDL_GPUBuffer* vertexBuffer,
           Uint64 vertexCount, SDL_GPUBuffer* indexBuffer, Uint64 indexCount,
           const std::string& description) {
    name_ = name;
    description_ = description;
    vertexBuffer_ = vertexBuffer;
    indexBuffer_ = indexBuffer;
    vertexCount_ = vertexCount;
    indexCount_ = indexCount;
}

const std::string& Mesh::getMeshName() const { return name_; }

const std::string& Mesh::getMeshDescription() const { return description_; }