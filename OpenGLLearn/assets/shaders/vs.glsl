#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec2 aUV;

layout(std140, binding = 0) uniform SceneConstants
{
    mat4 world;
    mat4 view;
    mat4 proj;
    vec4 eye;
    vec4 lightPos;
    vec3 timeRes;
    float pad;
};


layout(location = 0) out vec4 vNormal;
layout(location = 1) out vec4 vTangent;
layout(location = 2) out vec2 vUV;
layout(location = 3) out vec3 vRay;
layout(location = 4) out vec3 vLightDir;

void main()
{
    int sizeX = 10;
    int sizeY = 10;

    int x = gl_InstanceID % sizeX;
    int y = (gl_InstanceID / sizeX) % sizeY;
    int z = gl_InstanceID / (sizeX * sizeY);

    vec4 offset = vec4(
        float(x) * 10.0,
        float(y) * 10.0,
        float(z) * 10.0,
        0.0
    );

    vec4 worldPos = world * vec4(aPosition, 1.0);
    worldPos += offset;
    gl_Position = proj * view * worldPos;
    vNormal = world * vec4(aNormal,0.0);
    vTangent = world * vec4(aTangent,0.0);
    vUV = aUV;
    vUV.y *= -1;
    vRay = worldPos.xyz - eye.xyz;
    vLightDir = lightPos.xyz - worldPos.xyz;
}