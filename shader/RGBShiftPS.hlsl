#include "Common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    float2 uv = In.TexCoord;
    outDiffuse.r = g_Texture.Sample(g_SamplerState, float2(uv.x + Parameter.x, uv.y + Parameter.y)).r;
    outDiffuse.g = g_Texture.Sample(g_SamplerState, float2(uv.x, uv.y)).g;
    outDiffuse.b = g_Texture.Sample(g_SamplerState, float2(uv.x - Parameter.x, uv.y - Parameter.y)).b;
    outDiffuse.a = g_Texture.Sample(g_SamplerState, float2(uv.x, uv.y)).a;
    outDiffuse *= In.Diffuse;
}