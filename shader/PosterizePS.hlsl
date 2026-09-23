#include "Common.hlsl"

Texture2D g_Texture : register(t0); //0番テクスチャ
SamplerState g_SamplerState : register(s0);

// 適当なSeed値をもとに疑似乱数を計算する
float Rand(float2 coord)
{
    // frac(x) = x の小数部を返す　乱数は0.0 <= rand < 1.0の範囲
    return frac(sin(dot(coord.xy, float2(12.9898f, 78.233f))) * 43758.5453f);
}

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    float4 col = g_Texture.Sample(g_SamplerState, In.TexCoord);
    col.rgb *= In.Diffuse.rgb;
    
    //　コントラスト
    col.rgb = pow(saturate(col), Parameter.w);
    
    // ノイズ
    col.rgb += clamp(Rand(In.TexCoord * 300.0f), Parameter.y, Parameter.z);
    
    float div = 1.0f / Parameter.x; // 1段辺りの幅
    col.rgb = floor(col / div) * div; // 幅の値を使って色の値を求める
    col.rgb = saturate(col);
    
    outDiffuse.rgb = col.rgb;
    outDiffuse.a = col.a * In.Diffuse.a;
}