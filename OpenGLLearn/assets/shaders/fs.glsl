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

vec3 hsvToRgb(float h, float s, float v) {
    // h: 0.0 - 360.0, s: 0.0 - 1.0, v: 0.0 - 1.0
    float c = s; // float c = v * s;
    float h2 = h / 60.0;
    float x = c * (1.0 - abs(mod(h2, 2.0) - 1.0));
    vec3 rgb = (v - c) * vec3(1.0, 1.0, 1.0);

    if (0.0 <= h2 && h2 < 1.0) {
        rgb += vec3(c, x, 0.0);
    } else if (1.0 <= h2 && h2 < 2.0) {
        rgb += vec3(x, c, 0.0);
    } else if (2.0 <= h2 && h2 < 3.0) {
        rgb += vec3(0.0, c, x);
    } else if (3.0 <= h2 && h2 < 4.0) {
        rgb += vec3(0.0, x, c);
    } else if (4.0 <= h2 && h2 < 5.0) {
        rgb += vec3(x, 0.0, c);
    } else if (5.0 <= h2 && h2 < 6.0) {
        rgb += vec3(c, 0.0, x);
    }

    return rgb;
}

void main()
{
    //前方からの平行光源とする
    vec3 n = normalize(vNormal.xyz);
    vec3 v = normalize(-vRay);
    vec3 light = vec3(0,0,-1);
    vec4 texCol = texture(mainTexture,vUV);
    float diffuseB = saturate(dot(light, n));
    vec3 refLight = normalize(reflect(-light, n)); 
    float specularB = pow(saturate(dot(refLight, v)), 32);
    vec3 ambient = texCol.rgb * 0.1;
    vec3 diffuse = texCol.rgb * diffuseB;
    vec3 specular = vec3(1.0) * specularB;

    outColor = vec4(ambient + diffuse + specular, texCol.a);
    outColor *= vec4(hsvToRgb(abs(sin(timeRes.x * 5)) * 360,1.0,1.0),1.0) * 10;
}