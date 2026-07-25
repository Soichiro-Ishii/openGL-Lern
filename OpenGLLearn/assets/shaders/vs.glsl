#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec2 aUV;

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
};

layout(std430, binding = 0) readonly buffer InstanceBuffer
{
    InstanceData instances[];
};

layout(location = 0) out vec4 vNormal;
layout(location = 1) out vec4 vTangent;
layout(location = 2) out vec2 vUV;
layout(location = 3) out vec3 vRay;
layout(location = 4) out vec3 vLightDir;

void main()
{
    vec4 worldPos = instances[gl_InstanceID].world * vec4(aPosition, 1.0);
    gl_Position = proj * view * worldPos;
    vNormal = instances[gl_InstanceID].world * vec4(aNormal,0.0);
    vTangent = instances[gl_InstanceID].world * vec4(aTangent,0.0);
    vUV = aUV;
    vUV.y *= -1;
    vRay = worldPos.xyz - eye.xyz;
    vLightDir = lightPos.xyz - worldPos.xyz;
}