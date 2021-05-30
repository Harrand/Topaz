#version 450
#pragma shader_stage(vertex)

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

layout(binding = 0) uniform MVP
{
    mat4 model;
    mat4 view;
    mat4 projection;
} mvp;

void main()
{
    mat4 mvp_matrix = mvp.projection * mvp.view * mvp.model;
    gl_Position = mvp_matrix * vec4(position, 1.0);
}