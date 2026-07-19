#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

layout(std140, binding = 0) uniform SceneConstants
{
    mat4 world;
    mat4 view;
    mat4 proj;
    vec4 eye;
    vec3 timeRes;
    float pad;
};


layout(location = 0) out vec4 vNormal;
layout(location = 1) out vec2 vUV;
layout(location = 2) out vec3 vRay;

void main()
{
    vec4 worldPos = world * vec4(aPosition, 1.0);
    gl_Position = proj * view * worldPos;
    vNormal = world * vec4(aNormal,0.0);
    vUV = aUV;
    vUV.y *= -1;
    vRay = normalize(worldPos.xyz - eye.xyz);
}