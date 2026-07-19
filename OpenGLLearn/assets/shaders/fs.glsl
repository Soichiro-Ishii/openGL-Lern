#version 460 core
#define saturate(x) clamp(x, 0.0, 1.0)

layout(location = 0) in vec4 vNormal;
layout(location = 1) in vec2 vUV;
layout(location = 2) in vec3 vRay;

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
    vec3 n = normalize(vNormal.xyz);
    vec3 v = normalize(vRay);
    vec3 light = vec3(0,0,-1);
    vec4 texCol = texture(mainTexture,vUV);
    float diffuseB = saturate(dot(light, n));
    vec3 refLight = normalize(reflect(light, n)); 
    float specularB = pow(saturate(dot(refLight, v)), 32);
    vec3 ambient = texCol.rgb * 0.1;
    vec3 diffuse = texCol.rgb * diffuseB;
    vec3 specular = vec3(1.0) * specularB;

    outColor = vec4(ambient + diffuse + specular, texCol.a);
}