#version 460 core
#define saturate(x) clamp(x, 0.0, 1.0)

layout(location = 0) in vec4 vNormal;
layout(location = 1) in vec4 vTangent;
layout(location = 2) in vec2 vUV;
layout(location = 3) in vec3 vRay;
layout(location = 4) in vec3 vLightDir;

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
layout(binding = 1) uniform sampler2D nightTexture;
layout(binding = 2) uniform sampler2D normalMap;
layout(binding = 3) uniform sampler2D specularMap;

layout(location = 0) out vec4 outColor;

void main()
{
    // ------------------------------------------------------------
    // 法線マッピング
    // ------------------------------------------------------------

    vec3 geometryN = normalize(vNormal.xyz);
    vec3 t = normalize(vTangent.xyz);

    // 接線を法線に対して直交化
    t = normalize(t - geometryN * dot(geometryN, t));

    // vTangent.w に符号を格納していない場合は、
    // この "* vTangent.w" を削除する
    vec3 b = normalize(cross(geometryN, t)) * vTangent.w;

    mat3 TBN = mat3(t, b, geometryN);

    vec3 normalTS = texture(normalMap, vUV).rgb;
    normalTS = normalize(normalTS * 2.0 - 1.0);

    // DirectX形式のノーマルマップなら有効化
    // normalTS.y = -normalTS.y;

    vec3 n = normalize(TBN * normalTS);


    // ------------------------------------------------------------
    // 視線・ライト
    // ------------------------------------------------------------

    vec3 v = normalize(-vRay);

    // 表面から太陽へ向かう方向
    vec3 light = normalize(vLightDir);


    // ------------------------------------------------------------
    // テクスチャ
    // ------------------------------------------------------------

    vec4 mainTexCol =
        texture(mainTexture, vUV);

    vec3 nightTexCol =
        texture(nightTexture, vUV).rgb;

    float specMap =
        texture(specularMap, vUV).r;


    // ------------------------------------------------------------
    // 昼夜判定
    // ------------------------------------------------------------

    // 昼夜の大きな境界はノーマルマップを使わず、
    // 球本来の法線で判定する
    float geometryNdotL =
        dot(geometryN, light);

    // 細かい地表の照明はノーマルマップ込み
    float diffuseB =
        max(dot(n, light), 0.0);


    // ------------------------------------------------------------
    // 地表
    // ------------------------------------------------------------

    // 明暗境界を少し広げて滑らかにする
    float surfaceDayMask =
        smoothstep(-0.12, 0.12, geometryNdotL);

    // 昼側
    vec3 daySurface =
        mainTexCol.rgb *
        (0.01 + diffuseB);

    // 夜側の地表。
    // 完全な黒にせず、昼テクスチャをかなり弱く残す
    vec3 nightSurface =
        mainTexCol.rgb *
        vec3(0.002, 0.003, 0.006);

    vec3 surface =
        mix(
            nightSurface,
            daySurface,
            surfaceDayMask
        );


    // ------------------------------------------------------------
    // 都市光
    // ------------------------------------------------------------

    // 夜テクスチャの輝度
    float nightLuma =
        dot(
            nightTexCol,
            vec3(0.2126, 0.7152, 0.0722)
        );

    // 暗い背景を除去して都市光だけを残す
    float cityPixelMask =
        smoothstep(
            0.02,
            0.08,
            nightLuma
        );

    vec3 cityColor =
        max(
            nightTexCol - vec3(0.01),
            vec3(0.0)
        );

    cityColor *=
        cityPixelMask *
        3.0;

    // 都市光は太陽が地平線より少し下に沈んでから表示
    float cityNightMask =
        1.0 -
        smoothstep(
            -0.15,
            0.15,
            geometryNdotL
        );

    vec3 emission =
        cityColor *
        cityNightMask;


    // ------------------------------------------------------------
    // 鏡面反射
    // ------------------------------------------------------------

    vec3 refLight =
        reflect(-light, n);

    float specularB =
        pow(
            max(dot(refLight, v), 0.0),
            32.0
        );

    // 裏側にスペキュラが出るのを防ぐ
    float specularDayMask =
        smoothstep(
            0.0,
            0.03,
            geometryNdotL
        );

    vec3 specular =
        vec3(1.0) *
        specularB *
        specMap *
        specularDayMask;


    // ------------------------------------------------------------
    // 最終出力
    // ------------------------------------------------------------

    vec3 finalColor =
        surface +
        emission +
        specular;

    outColor =
        vec4(
            finalColor,
            mainTexCol.a
        );
}