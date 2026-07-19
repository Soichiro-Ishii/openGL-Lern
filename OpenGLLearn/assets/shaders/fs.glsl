#version 460 core

layout(location = 0) in vec4 vNormal;
layout(location = 1) in vec2 vUV;

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
    //前方からの平行光源とする
    vec3 light = vec3(0,0,-1);
    vec4 col = texture(mainTexture,vUV);
    col *= dot(light,vNormal.xyz);
    outColor = col;
}