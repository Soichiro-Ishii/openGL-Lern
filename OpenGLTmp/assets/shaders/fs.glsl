#version 460 core

layout(location = 0) in vec2 vUV;

layout(std140, binding = 0) uniform SceneConstants
{
    mat4 world;
    mat4 view;
    mat4 proj;
    vec4 eye;
    float time;
};

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(vUV.x, vUV.y, -cos(time * 0.25) * vUV.x + -sin(time * 0.25) * vUV.y, 1.0);
}