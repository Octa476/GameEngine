#version 460

layout (location = 0) in vec3 a_position;
layout (location = 0) out vec4 v_color;

// Uniform properties
layout(std140, set = 1, binding = 0) uniform UniformBlock {
    mat4 Model;
    mat4 View;
    mat4 Projection;
    vec4 a_color;
};

void main()
{
    gl_Position = Projection * View * Model * vec4(a_position, 1.0f);
    v_color = a_color;
}