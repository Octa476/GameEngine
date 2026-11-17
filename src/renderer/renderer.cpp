#include "renderer.hpp"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"
#include "core/log.hpp"
#include <filesystem>

bool Renderer::init(SDL_Window *window)
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

    // Graphical pipeline code, OpenGL all over again.
    SDL_GPUTransferBuffer* transferBuffer;
    // the vertex input layout
    struct Vertex
    {
        float x, y, z;    // vec3 position
        float r, g, b, a; // vec4 color
    };

    // a list of vertices
    static Vertex vertices[]{
        {0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},   // top vertex
        {-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f}, // bottom left vertex
        {0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f}   // bottom right vertex
    };

    // load the vertex shader code
    size_t vertexCodeSize;
    std::string path_vertex = shader_dir + std::string("vertex.spv");
    LOG_INFO("{}", path_vertex.data());
    void *vertexCode = SDL_LoadFile(path_vertex.data(), &vertexCodeSize);
    LOG_FATAL("{}", SDL_GetError());
    LOG_INFO("CREATED THE VERTEXCODE");
    LOG_INFO("Current path is {}", std::filesystem::current_path().c_str()); // (1)


    // create the vertex shader
    SDL_GPUShaderCreateInfo vertexInfo{};
    vertexInfo.code = (Uint8 *)vertexCode;
    vertexInfo.code_size = vertexCodeSize;
    vertexInfo.entrypoint = "main";
    vertexInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    vertexInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX;
    vertexInfo.num_samplers = 0;
    vertexInfo.num_storage_buffers = 0;
    vertexInfo.num_storage_textures = 0;
    vertexInfo.num_uniform_buffers = 0;

    SDL_GPUShader *vertexShader = SDL_CreateGPUShader(device_, &vertexInfo);

    // free the file
    SDL_free(vertexCode);

    // load the fragment shader code
    size_t fragmentCodeSize;
    std::string path_fragment = shader_dir + std::string("fragment.spv");
    LOG_INFO("{}", path_fragment.data());
    void *fragmentCode = SDL_LoadFile(path_fragment.data(), &fragmentCodeSize);
    LOG_INFO("CREATED THE VERTEXCODE");

    // create the fragment shader
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

    // free the file
    SDL_free(fragmentCode);
    LOG_INFO("CREATED THE FRAGMENTCODE");

    // create the graphics pipeline
    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    // describe the vertex buffers
    SDL_GPUVertexBufferDescription vertexBufferDesctiptions[1];
    vertexBufferDesctiptions[0].slot = 0;
    vertexBufferDesctiptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDesctiptions[0].instance_step_rate = 0;
    vertexBufferDesctiptions[0].pitch = sizeof(Vertex);

    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDesctiptions;

    // describe the vertex attribute
    SDL_GPUVertexAttribute vertexAttributes[2];
    LOG_INFO("CREATED THE VERTEXCODE");

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

    // describe the color target
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
    LOG_INFO("CREATED THE VERTEXCODE");

    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;
    LOG_INFO("CREATED THE VERTEXCODE");

    // create the pipeline
    graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device_, &pipelineInfo);
    if (graphicsPipeline == NULL) {
        LOG_FATAL("{}", SDL_GetError());
    }
    LOG_INFO("Created the graphics pipeline!");

    // we don't need to store the shaders after creating the pipeline
    SDL_ReleaseGPUShader(device_, vertexShader);
    SDL_ReleaseGPUShader(device_, fragmentShader);

    // create the vertex buffer
    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size = sizeof(vertices);
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertexBuffer_ = SDL_CreateGPUBuffer(device_, &bufferInfo);

    // create a transfer buffer to upload to the vertex buffer
    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.size = sizeof(vertices);
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferBuffer = SDL_CreateGPUTransferBuffer(device_, &transferInfo);

    // fill the transfer buffer
    Vertex *data = (Vertex *)SDL_MapGPUTransferBuffer(device_, transferBuffer, false);

    SDL_memcpy(data, (void *)vertices, sizeof(vertices));

    // data[0] = vertices[0];
    // data[1] = vertices[1];
    // data[2] = vertices[2];

    SDL_UnmapGPUTransferBuffer(device_, transferBuffer);

    // start a copy pass
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device_);
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    // where is the data
    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = transferBuffer;
    location.offset = 0;

    // where to upload the data
    SDL_GPUBufferRegion region{};
    region.buffer = vertexBuffer_;
    region.size = sizeof(vertices);
    region.offset = 0;

    // upload the data
    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

    // end the copy pass
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);

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

    if (swapchainTexture == NULL)
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

    renderPass_ =
        SDL_BeginGPURenderPass(commandBuffer_, &colorTargetInfo, 1, NULL);

    // bind the pipeline
    SDL_BindGPUGraphicsPipeline(renderPass_, graphicsPipeline);

    // bind the vertex buffer
    SDL_GPUBufferBinding bufferBindings[1];
    bufferBindings[0].buffer = vertexBuffer_;
    bufferBindings[0].offset = 0;

    SDL_BindGPUVertexBuffers(renderPass_, 0, bufferBindings, 1);

    // issue a draw call
    SDL_DrawGPUPrimitives(renderPass_, 3, 1, 0, 0);

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
    if (device_)
    {
        SDL_DestroyGPUDevice(device_);
        LOG_TRACE("Released GPU device");
    }
}
