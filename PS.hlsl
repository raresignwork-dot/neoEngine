struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
};

// PSMain という名前に統一（App側の指定と合わせる）
float4 PSMain(VS_OUTPUT input) : SV_Target
{
    // return float4(0, 1, 0, 1); // 緑
    return float4(1, 0, 0, 1); // 赤（お好きな方を）
}