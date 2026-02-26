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
    float3 lightDir;
    float pad0;
    float3 cameraPos;
    float pad1;
};

// b2 : Bones
cbuffer BoneCB : register(b2)
{
    float4x4 bones[128];
};

// b3 : Material
cbuffer MaterialCB : register(b3)
{
    float4 baseColor;
    float3 diffuseColor;
    float shininess;

    float3 specularColor;
    float metallic;

    float roughness;
    int usePBR;
    int useTexture;
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

    float4 localPos = float4(input.position, 1.0f);
    float3 localNrm = input.normal;

    float4 skinnedPos = float4(0, 0, 0, 0);
    float3 skinnedNormal = float3(0, 0, 0);

    // ---- スキニング ----
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        uint index = input.boneIndices[i];
        float weight = input.boneWeights[i];

        if (weight > 0.0f && index < 128)
        {
            skinnedPos += mul(bones[index], localPos) * weight;
            skinnedNormal += mul(bones[index], float4(localNrm, 0.0f)).xyz * weight;
        }
    }

    float sumW = input.boneWeights.x + input.boneWeights.y +
                 input.boneWeights.z + input.boneWeights.w;

    if (sumW == 0.0f)
    {
        skinnedPos = localPos;
        skinnedNormal = localNrm;
    }
    else
    {
        skinnedPos /= sumW;
        skinnedNormal = normalize(skinnedNormal);
    }

    // ---- 行列変換（DirectX11 標準：matrix × vector）----
    float4 worldPos = mul(world, skinnedPos);
    float3 worldNormal = normalize(mul(world, float4(skinnedNormal, 0.0f)).xyz);

    float4 viewPos = mul(view, worldPos);
    float4 projPos = mul(projection, viewPos);

    output.Pos = projPos;
    output.WorldPos = worldPos.xyz;
    output.WorldNormal = worldNormal;
    output.uv = input.uv;

    return output;
}

// =========================
// ピクセルシェーダ
// =========================

float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
    return float4(1, 0, 1, 1); // マゼンタ固定
}
