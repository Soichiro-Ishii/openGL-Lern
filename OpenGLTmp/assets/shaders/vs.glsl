#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;

layout(std140, binding = 0) uniform SceneConstants
{
    mat4x4 world;
    mat4x4 view;
    mat4x4 proj;
    vec4 eye;
    float time;
};

layout(location = 0) out vec2 vUV;

void main()
{
    gl_Position = proj * view * world * vec4(aPosition, 1.0);
    vUV = aUV;
}