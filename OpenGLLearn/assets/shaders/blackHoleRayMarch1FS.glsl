#version 460 core

layout(location = 0) in vec2 vUV;
layout(location = 0) out vec4 outColor;

// GLSLではTexture2DとSamplerStateを合わせてsampler2Dとして扱う
layout(binding = 0) uniform sampler2D uSkyTexture;

// C++側:
// struct alignas(16) BlackHoleConstants
// {
//     glm::vec2 resolution;
//     float time;
//     float padding;
// };
layout(std140, binding = 0) uniform BlackHoleConstants
{
    vec2 uResolution;
    float uTime;
    float _padding;
};

#define APPLY_SSAA 1

const int MATERIAL_COUNT = 3;
const int MAX_STEPS = 1000;

const float MAX_DISTANCE = 50.0;
const float SURFACE_DISTANCE = 0.01;
const float PI = 3.14159265358979323846;

const float MASS = 0.125;
const float CAMERA_SPEED = 0.5;

const vec3 BLACK_HOLE_POSITION = vec3(0.0);

const float SKY_SATURATION = 1.5;
const float CLOUD_SIZE = 1.5;
const float CLOUD_DENSITY = 0.75;

const vec3 MATERIALS[MATERIAL_COUNT] = vec3[MATERIAL_COUNT]
(
    vec3(0.8, 0.4, 0.0), // オレンジ
    vec3(0.0, 0.0, 0.0), // ブラックホール
    vec3(0.5, 0.7, 0.9)  // 青白
);

const vec2 SSAA_OFFSETS[4] = vec2[4]
(
    vec2(-0.25, -0.25),
    vec2( 0.25, -0.25),
    vec2(-0.25,  0.25),
    vec2( 0.25,  0.25)
);

struct RayHit
{
    float t;
    int material;

    vec3 rayDirection;

    bool hit;
    float distance;

    vec4 cloudColor;
};

struct DistanceResult
{
    float distance;
    int material;

    float cloudDistance1;
    float cloudDistance2;
};

float saturate(float value)
{
    return clamp(value, 0.0, 1.0);
}

vec3 saturate(vec3 value)
{
    return clamp(value, vec3(0.0), vec3(1.0));
}

vec3 changeSaturation(vec3 color, float saturation)
{
    float luminance = dot(
        color,
        vec3(0.2126, 0.7152, 0.0722)
    );

    return saturate(
        mix(vec3(luminance), color, saturation)
    );
}

/*
    GLSLのmat2コンストラクタは列優先。

    次の値は数学的には、

        [ cos -sin ]
        [ sin  cos ]

    になる。
*/
mat2 rotation2D(float angle)
{
    float c = cos(angle);
    float s = sin(angle);

    return mat2(
         c, s,
        -s, c
    );
}

vec2 directionToAngle(vec3 direction)
{
    return vec2(
        atan(direction.z, direction.x),
        atan(direction.y, length(direction.xz))
    );
}

vec3 getSkyColor(vec3 rayDirection)
{
    vec2 angle = directionToAngle(rayDirection);

    vec2 uv;

    uv.x = angle.x / (2.0 * PI) + 0.5;
    uv.y = 0.5 - angle.y / PI;

    return texture(uSkyTexture, uv).rgb;
}

float sdSphere(vec3 position, float radius)
{
    return length(position) - radius;
}

vec3 getCloudCenter1()
{
    return vec3(
        cos(uTime * 1.5),
        0.0,
        sin(uTime * 1.5)
    ) * 3.0;
}

vec3 getCloudCenter2()
{
    vec3 position = vec3(
        cos(uTime) * 2.0,
        0.0,
        sin(uTime)
    ) * 5.0;

    position.yz = rotation2D(PI / 3.5) * position.yz;

    return position;
}

DistanceResult getSceneDistance(vec3 rayPosition)
{
    DistanceResult result;

    // 衝突対象はブラックホールのみ
    float schwarzschildRadius = 2.0 * MASS;

    result.distance = sdSphere(
        rayPosition - BLACK_HOLE_POSITION,
        schwarzschildRadius
    );

    result.material = 1;

    // 雲密度計算用。衝突距離とは分離する
    result.cloudDistance1 = sdSphere(
        rayPosition - getCloudCenter1(),
        1.0
    );

    result.cloudDistance2 = sdSphere(
        rayPosition - getCloudCenter2(),
        1.0
    );

    return result;
}

/*
    光子軌道のSchwarzschild近似。

    hは初期位置と初期方向から求めた角運動量ベクトル。
*/
vec3 calculateSchwarzschildAcceleration(
    vec3 radialDirection,
    float schwarzschildRadius,
    vec3 angularMomentum
)
{
    float radius = max(length(radialDirection), 1e-6);
    float angularMomentumSquared =
        dot(angularMomentum, angularMomentum);

    return
        -1.5
        * schwarzschildRadius
        * angularMomentumSquared
        * radialDirection
        / pow(radius, 5.0);
}

void accumulateCloud(
    float cloudDistance,
    vec3 cloudMaterial,
    float deltaTime,
    inout vec3 accumulatedColor,
    inout float accumulatedAlpha
)
{
    if (cloudDistance >= CLOUD_SIZE)
        return;

    float densityPosition =
        saturate(1.0 - cloudDistance / CLOUD_SIZE)
        * CLOUD_DENSITY;

    float density = smoothstep(
        0.0,
        1.0,
        densityPosition
    );

    float remainingAlpha = 1.0 - accumulatedAlpha;

    accumulatedColor +=
        density
        * cloudMaterial
        * remainingAlpha
        * deltaTime;

    accumulatedAlpha +=
        density
        * 0.05
        * remainingAlpha
        * deltaTime;
}

RayHit rayMarch(vec3 rayOrigin, vec3 rayDirection)
{
    RayHit result;

    result.t = 0.0;
    result.material = 1;
    result.rayDirection = rayDirection;
    result.hit = false;
    result.distance = MAX_DISTANCE;
    result.cloudColor = vec4(0.0);

    vec3 rayPosition = rayOrigin;

    float traveledDistance = 0.0;

    vec3 accumulatedCloudColor = vec3(0.0);
    float accumulatedCloudAlpha = 0.0;

    vec3 initialRadialDirection =
        rayPosition - BLACK_HOLE_POSITION;

    // Schwarzschild時空では保存量として扱える
    vec3 angularMomentum =
        cross(initialRadialDirection, rayDirection);

    DistanceResult scene = getSceneDistance(rayPosition);
    float distanceToSurface = scene.distance;

    for (int i = 0; i < MAX_STEPS; ++i)
    {
        vec3 radialDirection =
            rayPosition - BLACK_HOLE_POSITION;

        float radius = length(radialDirection);
        float schwarzschildRadius = 2.0 * MASS;

        vec3 acceleration =
            calculateSchwarzschildAcceleration(
                radialDirection,
                schwarzschildRadius,
                angularMomentum
            );

        /*
            ブラックホール付近では細かく、
            遠くでは大きく進む。
        */
        float minimumRadius =
            1.5 * schwarzschildRadius;

        float maximumRadius =
            10.0 * schwarzschildRadius;

        float minimumDelta =
            SURFACE_DISTANCE * schwarzschildRadius;

        float maximumDelta =
            SURFACE_DISTANCE
            * 500.0
            * schwarzschildRadius;

        float deltaTime = mix(
            minimumDelta,
            maximumDelta,
            smoothstep(
                minimumRadius,
                maximumRadius,
                radius
            )
        );

        // SDFの表面を飛び越えないための制限
        deltaTime = min(
            deltaTime,
            max(distanceToSurface, 0.0)
        );

        if (deltaTime <= 1e-7)
        {
            result.hit =
                distanceToSurface < SURFACE_DISTANCE;

            break;
        }

        rayDirection += acceleration * deltaTime;
        rayDirection = normalize(rayDirection);

        rayPosition += rayDirection * deltaTime;
        traveledDistance += deltaTime;

        scene = getSceneDistance(rayPosition);

        distanceToSurface = scene.distance;
        result.material = scene.material;

        accumulateCloud(
            scene.cloudDistance1,
            MATERIALS[0],
            deltaTime,
            accumulatedCloudColor,
            accumulatedCloudAlpha
        );

        accumulateCloud(
            scene.cloudDistance2,
            MATERIALS[2],
            deltaTime,
            accumulatedCloudColor,
            accumulatedCloudAlpha
        );

        if (distanceToSurface < SURFACE_DISTANCE)
        {
            result.hit = true;
            break;
        }

        if (traveledDistance > MAX_DISTANCE)
            break;
    }

    result.t = traveledDistance;
    result.rayDirection = rayDirection;
    result.distance = distanceToSurface;

    result.cloudColor = vec4(
        accumulatedCloudColor,
        accumulatedCloudAlpha
    );

    return result;
}

vec3 renderSample(vec2 uv)
{
    vec2 screenPosition = uv * 2.0 - 1.0;

    screenPosition.x *=
        uResolution.x / uResolution.y;

    /*
        HLSL版の画面座標系に合わせるための反転。

        上下が逆になった場合は、この行を消す。
    */
    screenPosition.y *= -1.0;

    float cameraAngle = uTime * CAMERA_SPEED;

    vec3 rayOrigin = vec3(
        sin(cameraAngle),
        0.0,
        -cos(cameraAngle)
    ) * 5.0;

    float fieldOfView = radians(90.0);
    float focalLength =
        1.0 / tan(fieldOfView * 0.5);

    vec3 rayDirection = normalize(
        vec3(screenPosition, focalLength)
    );

    rayDirection.xz =
        rotation2D(cameraAngle) * rayDirection.xz;

    RayHit hit = rayMarch(
        rayOrigin,
        rayDirection
    );

    vec3 cloudColor = hit.cloudColor.rgb;

    if (hit.hit)
    {
        return
            MATERIALS[hit.material]
            + cloudColor;
    }

    vec3 skyColor =
        getSkyColor(hit.rayDirection);

    skyColor = changeSaturation(
        skyColor,
        SKY_SATURATION
    );

    return skyColor + cloudColor;
}

void main()
{
    vec3 color = vec3(0.0);

#if APPLY_SSAA

    vec2 pixelUVSize = 1.0 / uResolution;

    for (int i = 0; i < 4; ++i)
    {
        vec2 sampleUV =
            vUV
            + SSAA_OFFSETS[i]
            * pixelUVSize;

        color += renderSample(sampleUV);
    }

    color *= 0.25;

#else

    color = renderSample(vUV);

#endif
    outColor = vec4(color, 1.0);
}