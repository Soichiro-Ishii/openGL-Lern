#version 460 core

layout(location = 0) in vec2 vUV;

layout(std140, binding = 0) uniform SceneConstants
{
    mat4 world;
    mat4 view;
    mat4 proj;
    vec4 eye;
    vec3 timeRes;
    float pad;
};

layout(binding = 0) uniform sampler2D mainTexture;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(mainTexture,vUV);
}