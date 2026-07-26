#version 460 core
layout(local_size_x = 256) in;

struct InstanceData
{
	mat4 world;
};

layout(std430, binding = 0) buffer InstanceBuffer
{
    InstanceData instances[];
};

layout(std140, binding = 0) uniform SceneConstants
{
    mat4 view;
    mat4 proj;
    vec4 eye;
    vec4 lightPos;
    vec3 timeRes;
    float pad;
};
layout(std140, binding = 1) uniform InstanceCount{
    uint instanceCount;
};

mat4 translationMatrix(vec3 offset) {
    return mat4(
        1.0, 0.0, 0.0, 0.0, // ‘æ1—ñ
        0.0, 1.0, 0.0, 0.0, // ‘æ2—ñ
        0.0, 0.0, 1.0, 0.0, // ‘æ3—ñ
        offset.x, offset.y, offset.z, 1.0 // ‘æ4—ñ
    );
}

// XŽ²Žü‚è‚Ì‰ñ“]
mat3 rotateX(float angle) {
    float s = sin(angle), c = cos(angle);
    return mat3(
        1.0, 0.0, 0.0,
        0.0,   c,  -s,
        0.0,   s,   c
    );
}

// YŽ²Žü‚è‚Ì‰ñ“]
mat3 rotateY(float angle) {
    float s = sin(angle), c = cos(angle);
    return mat3(
        c, 0.0, s,
        0.0, 1.0, 0.0,
       -s, 0.0, c
    );
}

// ZŽ²Žü‚è‚Ì‰ñ“]
mat3 rotateZ(float angle) {
    float s = sin(angle), c = cos(angle);
    return mat3(
        c,  -s, 0.0,
        s,   c, 0.0,
        0.0, 0.0, 1.0
    );
}

void main(){
    uint id = gl_GlobalInvocationID.x;
    if (id >= instanceCount)
        return;

    int sizeX = 10;
	int sizeY = 10;
    int x = int(id) % sizeX;
	int y = (int(id) / sizeX) % sizeY;
	int z = int(id) / (sizeX * sizeY);

    vec3 offset = vec3(
			float(x) * 10.0,
			float(y) * 10.0,
			float(z) * 10.0
	);

    instances[id].world = translationMatrix(offset) * mat4(rotateY(radians(-20 * timeRes.x)));
}