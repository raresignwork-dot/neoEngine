// =========================
// 定数バッファ
// =========================

// b0 : PerObject
cbuffer PerObjectCB : register(b0)
{
    float4x4 world;
};

// b1 : PerFrame
cbuffer PerFrameCB : register(b1)
{
    float4x4 view;
    float4x4 projection;
    float3 lightDir; // 簡易ライト
    float pad0;
    float3 cameraPos;
    float pad1;
};

// b2 : Bones
cbuffer BoneCB : register(b2)
{
    float4x4 boneMatrices[128];
};

// b3 : Material
cbuffer MaterialCB : register(b3)
{
    float4 baseColor; // RGBA
    float3 diffuseColor; // Phong 用
    float shininess; // Phong 用

    float3 specularColor; // Phong 用
    float metallic; // PBR 用

    float roughness; // PBR 用
    int usePBR; // 0 = Phong, 1 = PBR
    int useTexture; // 0 = 色のみ, 1 = テクスチャ使用
    int pad2;
};

// テクスチャ
Texture2D baseColorTex : register(t0);
SamplerState samLinear : register(s0);

// =========================
// 頂点シェーダ
// =========================

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;

    uint4 boneIndices : BLENDINDICES;
    float4 boneWeights : BLENDWEIGHT;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 WorldPos : TEXCOORD0;
    float3 WorldNormal : TEXCOORD1;
    float2 uv : TEXCOORD2;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;

    // ---- スキニング ----
    float4 skinnedPos = float4(input.position, 1.0f);
    float3 skinnedNormal = input.normal;

    if (any(input.boneWeights != 0))
    {
        float4 p = float4(0, 0, 0, 0);
        float3 n = float3(0, 0, 0);

        [unroll]
        for (int i = 0; i < 4; i++)
        {
            uint idx = input.boneIndices[i];
            float w = input.boneWeights[i];

            float4x4 m = boneMatrices[idx];

            p += mul(float4(input.position, 1.0f), m) * w;
            n += mul(float4(input.normal, 0.0f), m).xyz * w;
        }

        skinnedPos = p;
        skinnedNormal = normalize(n);
    }

    // ---- 通常の変換 ----
    float4 worldPos = mul(skinnedPos, world);
    float4 viewPos = mul(worldPos, view);
    float4 projPos = mul(viewPos, projection);

    float3 worldNormal = normalize(mul(float4(skinnedNormal, 0.0f), world).xyz);

    output.Pos = projPos;
    output.WorldPos = worldPos.xyz;
    output.WorldNormal = worldNormal;
    output.uv = input.uv;

    return output;
}

// =========================
// ピクセルシェーダ
// =========================

float3 BRDF_Phong(float3 N, float3 L, float3 V, float3 diffuseCol, float3 specCol, float shininess)
{
    float3 R = reflect(-L, N);
    float NdotL = saturate(dot(N, L));
    float RdotV = saturate(dot(R, V));

    float3 diffuse = diffuseCol * NdotL;
    float3 specular = specCol * pow(RdotV, shininess);

    return diffuse + specular;
}

float3 BRDF_PBR(float3 N, float3 L, float3 V, float3 baseCol, float metallic, float roughness)
{
    // 超簡易 PBR（本格的にするなら GGX 等を追加）
    float3 H = normalize(L + V);

    float NdotL = saturate(dot(N, L));
    float NdotV = saturate(dot(N, V));
    float NdotH = saturate(dot(N, H));

    float3 F0 = lerp(float3(0.04, 0.04, 0.04), baseCol, metallic);
    float3 F = F0 + (1.0 - F0) * pow(1.0 - NdotV, 5.0);

    float rough2 = roughness * roughness;
    float D = rough2 / max(0.001, (3.14159 * pow((NdotH * NdotH * (rough2 - 1.0) + 1.0), 2.0)));
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    float Gv = NdotV / (NdotV * (1.0 - k) + k);
    float Gl = NdotL / (NdotL * (1.0 - k) + k);
    float G = Gv * Gl;

    float3 specular = (D * F * G) / max(0.001, 4.0 * NdotL * NdotV);
    float3 kd = (1.0 - F) * (1.0 - metallic);
    float3 diffuse = kd * baseCol / 3.14159;

    return (diffuse + specular) * NdotL;
}

float4 PSMain(VS_OUTPUT input) : SV_Target
{
    float3 N = normalize(input.WorldNormal);
    float3 L = normalize(-lightDir);
    float3 V = normalize(cameraPos - input.WorldPos);

    float4 albedo = baseColor;
    if (useTexture)
    {
        albedo *= baseColorTex.Sample(samLinear, input.uv);
    }

    float3 color;

    if (usePBR == 1)
    {
        color = BRDF_PBR(N, L, V, albedo.rgb, metallic, roughness);
    }
    else
    {
        color = BRDF_Phong(N, L, V, diffuseColor, specularColor, shininess);
    }

    return float4(color, albedo.a);
}
