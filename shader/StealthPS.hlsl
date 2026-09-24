
#include "Common.hlsl"

Texture2D g_Texture1 : register(t0); //テクスチャ0番
Texture2D g_Texture2 : register(t1); // テクスチャ1番
SamplerState g_SamplerState : register(s0); // テクスチャサンプラー0番

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    //テクセルの色を乗算して出力
    outDiffuse = In.Diffuse;
    
    outDiffuse *= g_Texture2.SampleLevel(g_SamplerState, In.TexCoord, Parameter.x);
    
    outDiffuse *= In.Diffuse;
}