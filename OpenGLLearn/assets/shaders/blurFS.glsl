#version 460 core

layout(location = 0) in vec2 vUV;
layout(binding = 0) uniform sampler2D uTex;

layout(location = 0) out vec4 outColor;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(uTex, 0));

    vec3 color = vec3(0.0);

    color += texture(uTex, vUV + texelSize * vec2(-1.0, -1.0)).rgb;
    color += texture(uTex, vUV + texelSize * vec2( 0.0, -1.0)).rgb;
    color += texture(uTex, vUV + texelSize * vec2( 1.0, -1.0)).rgb;

    color += texture(uTex, vUV + texelSize * vec2(-1.0,  0.0)).rgb;
    color += texture(uTex, vUV).rgb;
    color += texture(uTex, vUV + texelSize * vec2( 1.0,  0.0)).rgb;

    color += texture(uTex, vUV + texelSize * vec2(-1.0,  1.0)).rgb;
    color += texture(uTex, vUV + texelSize * vec2( 0.0,  1.0)).rgb;
    color += texture(uTex, vUV + texelSize * vec2( 1.0,  1.0)).rgb;

    color /= 9.0;

    outColor = vec4(color, 1.0);
}