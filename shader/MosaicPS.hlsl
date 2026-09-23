#include "Common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    float2 uv = In.TexCoord;
    float2 center = float2(0.5f, 0.5f);
    float2 distanceUV = uv - center;

    distanceUV.x *= Parameter.x / Parameter.y;

    if (length(distanceUV) < Parameter.w)
    {
        uv *= Parameter.xy;
        uv /= Parameter.z;
        uv = floor(uv) * Parameter.z;
        uv /= Parameter.xy;
    }

    uv = clamp(uv, 0.0f, 1.0f);

    outDiffuse = g_Texture.SampleLevel(g_SamplerState, uv, 0.0f);
    outDiffuse *= In.Diffuse;
}