#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "renderer.hpp"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"
#include "core/log.hpp"
#include "../geometry/formats.hpp"
#include "../geometry/3D_objects.hpp"
#include "render_context/render_context.hpp"

bool Renderer::init(SDL_Window *window, RenderCommandList &commandList)
{
    device_ = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV |
                                      SDL_GPU_SHADERFORMAT_MSL |
                                      SDL_GPU_SHADERFORMAT_DXIL,
                                  false, NULL);

    if (!device_)
    {
        LOG_FATAL("{}", SDL_GetError());
        return false;
    }

    LOG_INFO("GPU backend: {}", SDL_GetGPUDeviceDriver(device_));

    if (!SDL_ClaimWindowForGPUDevice(device_, window))
    {
        LOG_FATAL("{}", SDL_GetError());
        return false;
    }

    // Excute init commands, like creating meshes and graphics pipelines.
    executeCommands(window, commandList);
    
    return true;
}

SDL_GPUBuffer *Renderer::createVertexBuffer_(const std::vector<Formats::Vertex> &vertices)
{
    // Create the vertex buffer.
    const Uint32 bufferSize = vertices.size() * sizeof(Formats::Vertex);
    SDL_GPUBuffer *vertexBuffer;
    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size = bufferSize;
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertexBuffer = SDL_CreateGPUBuffer(device_, &bufferInfo);
    if (!vertexBuffer)
    {
        LOG_ERROR("VertexBuffer creation failed: {}", SDL_GetError());
        return nullptr;
    }

    // Create a transfer buffer to upload to the vertex buffer.
    SDL_GPUTransferBuffer *transferBuffer;
    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.size = bufferSize;
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferBuffer = SDL_CreateGPUTransferBuffer(device_, &transferInfo);
    if (!transferBuffer)
    {
        LOG_ERROR("TransferBuffer creation failed: {}", SDL_GetError());
        return nullptr;
    }

    // Fill the transfer buffer.
    Formats::Vertex *data = static_cast<Formats::Vertex *>(SDL_MapGPUTransferBuffer(device_, transferBuffer, false));
    if (!data)
    {
        LOG_ERROR("SDL_MapGPUTransferBuffer failed: {}", SDL_GetError());
        return nullptr;
    }

    SDL_memcpy(data, static_cast<const void *>(vertices.data()), bufferSize);

    SDL_UnmapGPUTransferBuffer(device_, transferBuffer);

    // Start a copy pass.
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device_);
    if (!commandBuffer)
    {
        LOG_ERROR("SDL_AcquireGPUCommandBuffer failed: {}", SDL_GetError());
        return nullptr;
    }

    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    if (!copyPass)
    {
        LOG_ERROR("SDL_BeginGPUCopyPass failed: {}", SDL_GetError());
        return nullptr;
    }

    // Where is the data.
    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = transferBuffer;
    location.offset = 0;

    // Where to upload the data.
    SDL_GPUBufferRegion region{};
    region.buffer = vertexBuffer;
    region.size = bufferSize;
    region.offset = 0;

    // Upload the data.
    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

    // End the copy pass.
    SDL_EndGPUCopyPass(copyPass);
    if (!SDL_SubmitGPUCommandBuffer(commandBuffer))
    {
        LOG_ERROR("SDL_SubmitGPUCommandBuffer failed: {}", SDL_GetError());
        return nullptr;
    }

    // Release the transfer buffer.
    SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);

    return vertexBuffer;
}

SDL_GPUBuffer *Renderer::createIndexBuffer_(const std::vector<Formats::Triangle> &triangles)
{
    // Create the index buffer.
    const Uint32 bufferSize = static_cast<Uint32>(triangles.size()) * static_cast<Uint32>(sizeof(Formats::Triangle));
    SDL_GPUBuffer *indexBuffer;
    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size = bufferSize;
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    indexBuffer = SDL_CreateGPUBuffer(device_, &bufferInfo);
    if (!indexBuffer)
    {
        LOG_ERROR("IndexBuffer creation failed: {}", SDL_GetError());
        return nullptr;
    }

    // Create a transfer buffer to upload to the vertex buffer.
    SDL_GPUTransferBuffer *transferBuffer;
    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.size = bufferSize;
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferBuffer = SDL_CreateGPUTransferBuffer(device_, &transferInfo);
    if (!transferBuffer)
    {
        LOG_ERROR("TransferBuffer creation failed: {}", SDL_GetError());
        return nullptr;
    }

    // Fill the transfer buffer.
    Formats::Triangle *data = static_cast<Formats::Triangle *>(SDL_MapGPUTransferBuffer(device_, transferBuffer, false));
    if (!data)
    {
        LOG_ERROR("SDL_MapGPUTransferBuffer failed: {}", SDL_GetError());
        return nullptr;
    }

    SDL_memcpy(data, static_cast<const void *>(triangles.data()), bufferSize);

    SDL_UnmapGPUTransferBuffer(device_, transferBuffer);

    // Start a copy pass.
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device_);
    if (!commandBuffer)
    {
        LOG_ERROR("SDL_AcquireGPUCommandBuffer failed: {}", SDL_GetError());
        return nullptr;
    }

    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    if (!copyPass)
    {
        LOG_ERROR("SDL_BeginGPUCopyPass failed: {}", SDL_GetError());
        return nullptr;
    }

    // Where is the data.
    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = transferBuffer;
    location.offset = 0;

    // Where to upload the data.
    SDL_GPUBufferRegion region{};
    region.buffer = indexBuffer;
    region.size = bufferSize;
    region.offset = 0;

    // Upload the data.
    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

    // End the copy pass.
    SDL_EndGPUCopyPass(copyPass);
    if (!SDL_SubmitGPUCommandBuffer(commandBuffer))
    {
        LOG_ERROR("SDL_SubmitGPUCommandBuffer failed: {}", SDL_GetError());
        return nullptr;
    }
    // Release the transfer buffer.
    SDL_ReleaseGPUTransferBuffer(device_, transferBuffer);

    return indexBuffer;
}

std::optional<MeshId> Renderer::createMesh_(const MeshProperties &properties)
{
    const Formats::Shape &shape = properties.shape;

    // Create GPU buffers.
    SDL_GPUBuffer *vertexBuffer = createVertexBuffer_(shape.vertexes);
    if (!vertexBuffer)
        return std::nullopt;
    Uint64 vertexCount = shape.vertexes.size();

    SDL_GPUBuffer *indexesBuffer = createIndexBuffer_(shape.triangles);
    if (!indexesBuffer)
        return std::nullopt;
    Uint64 indexCount = sizeof(Formats::Triangle) * shape.triangles.size();

    // Create a mesh and saved it in the map.
    MeshId mesh_id = static_cast<MeshId>(stringHasher_(shape.name));
    mesh_map_[mesh_id] = std::make_unique<Mesh>(shape.name,
                                                vertexBuffer,
                                                vertexCount,
                                                indexesBuffer,
                                                indexCount);
    return mesh_id;
}

// Graphical pipeline code, OpenGL all over again.
std::optional<GraphicsId> Renderer::createGraphicsPipeline_(SDL_Window *window, const GraphicsProperties &properties)
{
    SDL_GPUPrimitiveType primitive_type;

    switch (properties.type)
    {
    case GraphicsType::Triangles:
        primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
        break;

    case GraphicsType::Lines:
        primitive_type = SDL_GPU_PRIMITIVETYPE_LINELIST;
    default:
        primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
        break;
    }

    // Load the vertex shader code.
    size_t vertexCodeSize;
    std::string path_vertex = shader_dir + std::string("vertex.spv");
    void *vertexCode = SDL_LoadFile(path_vertex.data(), &vertexCodeSize);
    if (!vertexCode)
    {
        LOG_FATAL("{}", SDL_GetError());
        return std::nullopt;
    }

    // Create the vertex shader.
    SDL_GPUShaderCreateInfo vertexInfo{};
    vertexInfo.code = (Uint8 *)vertexCode;
    vertexInfo.code_size = vertexCodeSize;
    vertexInfo.entrypoint = "main";
    vertexInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    vertexInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX;
    vertexInfo.num_samplers = 0;
    vertexInfo.num_storage_buffers = 0;
    vertexInfo.num_storage_textures = 0;
    vertexInfo.num_uniform_buffers = 1;

    SDL_GPUShader *vertexShader = SDL_CreateGPUShader(device_, &vertexInfo);
    if (!vertexShader)
    {
        LOG_FATAL("{}", SDL_GetError());
        return std::nullopt;
    }

    // Free the file.
    SDL_free(vertexCode);

    // Load the fragment shader code.
    size_t fragmentCodeSize;
    std::string path_fragment = shader_dir + std::string("fragment.spv");
    void *fragmentCode = SDL_LoadFile(path_fragment.data(), &fragmentCodeSize);
    if (!fragmentCode)
    {
        LOG_FATAL("{}", SDL_GetError());
        return std::nullopt;
    }

    // Create the fragment shader.
    SDL_GPUShaderCreateInfo fragmentInfo{};
    fragmentInfo.code = (Uint8 *)fragmentCode;
    fragmentInfo.code_size = fragmentCodeSize;
    fragmentInfo.entrypoint = "main";
    fragmentInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    fragmentInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    fragmentInfo.num_samplers = 0;
    fragmentInfo.num_storage_buffers = 0;
    fragmentInfo.num_storage_textures = 0;
    fragmentInfo.num_uniform_buffers = 0;

    SDL_GPUShader *fragmentShader = SDL_CreateGPUShader(device_, &fragmentInfo);
    if (!fragmentShader)
    {
        LOG_FATAL("{}", SDL_GetError());
        return std::nullopt;
    }

    // Free the file.
    SDL_free(fragmentCode);
    LOG_INFO("Vertex and fragment shaders successfully created!");

    // Create the graphics pipeline.
    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type = primitive_type;

    // Describe the vertex buffers.
    SDL_GPUVertexBufferDescription vertexBufferDesctiptions[1];
    vertexBufferDesctiptions[0].slot = 0;
    vertexBufferDesctiptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDesctiptions[0].instance_step_rate = 0;
    vertexBufferDesctiptions[0].pitch = sizeof(Vertex);

    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDesctiptions;

    // Describe the vertex attribute
    SDL_GPUVertexAttribute vertexAttributes[2];

    // a_position
    vertexAttributes[0].buffer_slot = 0;
    vertexAttributes[0].location = 0;
    vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    vertexAttributes[0].offset = 0;

    // a_color
    vertexAttributes[1].buffer_slot = 0;
    vertexAttributes[1].location = 1;
    vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    vertexAttributes[1].offset = sizeof(float) * 3;

    pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
    pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;

    // Describe the color target.
    SDL_GPUColorTargetDescription colorTargetDescriptions[1];
    colorTargetDescriptions[0] = {};
    colorTargetDescriptions[0].blend_state.enable_blend = true;
    colorTargetDescriptions[0].blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions[0].blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(device_, window);

    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;

    // Create the pipeline.
    SDL_GPUGraphicsPipeline *graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device_, &pipelineInfo);
    if (!graphicsPipeline)
    {
        LOG_FATAL("{}", SDL_GetError());
        return std::nullopt;
    }
    else
    {
        LOG_INFO("Graphics pipeline successfully created!");
    }

    // We don't need to store the shaders after creating the pipeline.
    SDL_ReleaseGPUShader(device_, vertexShader);
    SDL_ReleaseGPUShader(device_, fragmentShader);

    GraphicsId graphics_id = static_cast<MeshId>(stringHasher_(properties.name));
    graphics_pipeline_map_[graphics_id] = graphicsPipeline;

    return graphics_id;
}

bool Renderer::executeCommands(SDL_Window *window, RenderCommandList &commandList)
{
    std::vector<MeshProperties> &createMeshList = commandList.createMeshList;
    std::vector<MeshHandler> &resultMeshList = commandList.resultMeshList;

    for (MeshProperties &prop : createMeshList)
    {
        auto mesh_id = createMesh_(prop);
        if (mesh_id.has_value())
        {
            resultMeshList.push_back(MeshHandler{.generation_id = prop.generation_id, .mesh_id = mesh_id.value()});
        }
        else
        {
            createMeshList.clear();
            return false;
        }
    }
    createMeshList.clear();

    std::vector<GraphicsProperties> &createGraphicsList = commandList.createGraphicsList;
    std::vector<GraphicsHandler> &resultGraphicsList = commandList.resultGrapicsList;

    for (GraphicsProperties &prop : createGraphicsList)
    {
        auto graphics_id = createGraphicsPipeline_(window, prop);
        if (graphics_id.has_value())
        {
            resultGraphicsList.push_back(GraphicsHandler{.generation_id = prop.generation_id, .graphics_id = graphics_id.value()});
        }
        else
        {
            createGraphicsList.clear();
            return false;
        }
    }
    createGraphicsList.clear();
    return true;
}

bool Renderer::beginFrame(SDL_Window *window)
{
    commandBuffer_ = SDL_AcquireGPUCommandBuffer(device_);

    if (!commandBuffer_)
    {
        LOG_FATAL("{}", SDL_GetError());
        return false;
    }

    SDL_GPUTexture *swapchainTexture{nullptr};

    SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer_, window,
                                          &swapchainTexture, nullptr, nullptr);

    if (!swapchainTexture)
    {
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

    renderPass_ = SDL_BeginGPURenderPass(commandBuffer_, &colorTargetInfo, 1, NULL);
    if (!renderPass_)
    {
        LOG_ERROR("SDL_BeginGPURenderPass failed: {}", SDL_GetError());
        return false;
    }

    return true;
}

bool Renderer::render(SDL_Window *window, const RenderCameraInfo &cameraInfo, RenderDrawList &drawList)
{
    graphicsPipeline_ = nullptr;
    for (game::Object &obj : drawList.drawList)
    {
        auto graphics_pipeline_it = graphics_pipeline_map_.find(obj.graphicsId);
        if (graphics_pipeline_it == graphics_pipeline_map_.end())
        {
            LOG_ERROR("Graphics pipeline for {}", obj.to_string());
            continue;
        }

        if (graphics_pipeline_it->second != graphicsPipeline_)
        {
            graphicsPipeline_ = graphics_pipeline_it->second;
            // Bind the pipeline.
            SDL_BindGPUGraphicsPipeline(renderPass_, graphicsPipeline_);
        }


        auto mesh_it = mesh_map_.find(obj.meshId); 
        if (mesh_it == mesh_map_.end())
        {
            LOG_ERROR("Mesh not found for {}", obj.to_string());
            continue;
        }

        Mesh *mesh = mesh_it->second.get();

        // Bind the vertex buffer.
        SDL_GPUBufferBinding bufferBindingsVertex[1];
        bufferBindingsVertex[0].buffer = mesh->vertexBuffer_;
        bufferBindingsVertex[0].offset = 0;

        SDL_BindGPUVertexBuffers(renderPass_, 0, bufferBindingsVertex, 1);

        // Bind the index buffer.
        SDL_GPUBufferBinding bufferBindingsIndex[1];
        bufferBindingsIndex[0].buffer = mesh->indexBuffer_;
        bufferBindingsIndex[0].offset = 0;

        SDL_BindGPUIndexBuffer(renderPass_, bufferBindingsIndex, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        // Push the uniform data to the GPU.
        UniformBuffer uniform_buffer{obj.model, cameraInfo.viewMatrix, cameraInfo.projectionMatrix, obj.color};
        SDL_PushGPUVertexUniformData(commandBuffer_, 0, &uniform_buffer, sizeof(UniformBuffer));

        // Issue a draw call.
        SDL_DrawGPUIndexedPrimitives(renderPass_, mesh->indexCount_, 1, 0, 0, 0);
    }

    return true;
}

bool Renderer::endFrame()
{
    SDL_EndGPURenderPass(renderPass_);
    renderPass_ = nullptr;

    auto ret = SDL_SubmitGPUCommandBuffer(commandBuffer_);
    commandBuffer_ = nullptr;

    if (!ret)
    {
        LOG_FATAL("SDL_SubmitGPUCommandBuffer failed: {}", SDL_GetError());
        return false;
    }

    return true;
}

Renderer::~Renderer()
{
    // Release buffers from all the meshes.
    for (auto it = mesh_map_.begin(); it != mesh_map_.end(); it = next(it))
    {
        SDL_GPUBuffer *vertexBuffer = it->second->vertexBuffer_;
        SDL_GPUBuffer *indexBuffer = it->second->indexBuffer_;

        SDL_ReleaseGPUBuffer(device_, vertexBuffer);
        SDL_ReleaseGPUBuffer(device_, indexBuffer);
    }

    if (device_)
    {
        // Release all pipelines.
        for (auto it = graphics_pipeline_map_.begin(); it != graphics_pipeline_map_.end(); it = next(it))
        {
            SDL_ReleaseGPUGraphicsPipeline(device_, it->second);
        }
        SDL_DestroyGPUDevice(device_);
        LOG_TRACE("Released GPU device");
    }
}
