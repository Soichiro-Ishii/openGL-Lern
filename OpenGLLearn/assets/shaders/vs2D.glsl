#version 460 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aNormal;
layout(location = 2) in vec2 aUV;

layout(std140, binding = 0) uniform SceneConstants
{
    mat4 view;
    mat4 proj;
    vec4 eye;
    vec4 lightPos;
    vec3 timeRes;
    float pad;
};

struct InstanceData
{
    mat4 world;
    vec4 col;
};

layout(std430, binding = 0) readonly buffer InstanceBuffer
{
    InstanceData instances[];
};

layout(location = 0) out vec2 vNormal;
layout(location = 1) out vec2 vUV;
layout(location = 2) out vec2 vRay;
layout(location = 3) out vec2 vLightDir;
layout(location = 4) out vec4 vColor;

void main()
{
    vec4 worldPos = instances[gl_InstanceID].world * vec4(aPosition,0.0, 1.0);
    gl_Position = proj * view * worldPos;
    vNormal = (instances[gl_InstanceID].world * vec4(aNormal,0.0,0.0)).xy;
    vUV = aUV;
    vUV.y = 1.0 - vUV.y;
    vRay = worldPos.xy - eye.xy;
    vLightDir = lightPos.xy - worldPos.xy;
    vColor = instances[gl_InstanceID].col;
}