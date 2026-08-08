#version 460 core
#define saturate(x) clamp(x, 0.0, 1.0)

layout(location = 0) in vec2 vNormal;
layout(location = 1) in vec2 vUV;
layout(location = 2) in vec2 vRay;
layout(location = 3) in vec2 vLightDir;
layout(location = 4) in vec4 vColor;

layout(std140, binding = 0) uniform SceneConstants
{
    mat4 view;
    mat4 proj;
    vec4 eye;
    vec4 lightPos;
    vec3 timeRes;
    float pad;
};

layout(binding = 0) uniform sampler2D mainTexture;

layout(location = 0) out vec4 outColor;

void main()
{
    vec2 n = normalize(vNormal);
    vec2 light = normalize(vLightDir);

    outColor = vColor * saturate(dot(light, n));
}