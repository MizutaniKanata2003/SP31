#include "Common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    float4 col = g_Texture.Sample(g_SamplerState, In.TexCoord);
    float mode = Parameter.x;
    float time = Parameter.y;
    float lum = max(col.r, max(col.g, col.b));
    float autoAlpha = saturate(lum * 2.2f) * In.Diffuse.a;

    // -------------------------------------------------------------
    // Mode 1: 魔法陣・神殿床面（ルーン発光＋同心円パルスライティング）
    // -------------------------------------------------------------
    if (mode > 0.5f && mode < 1.5f)
    {
        float2 uvCenter = In.TexCoord - float2(0.5f, 0.5f);
        float dist = length(uvCenter);
        float edgeMask = smoothstep(0.5f, 0.46f, dist);

        // 明るいルーン・金銀ラインの光をブルーム強調
        float runeGlow = smoothstep(0.45f, 0.85f, lum);
        float pulse = 0.8f + sin(time * 6.0f - dist * 12.0f) * 0.35f;
        col.rgb += float3(1.0f, 0.95f, 0.8f) * runeGlow * pulse * 1.5f;

        autoAlpha *= edgeMask;
    }
    // -------------------------------------------------------------
    // Mode 2: 神聖な長剣（ブレードの鋭利な白金リムライト強調）
    // -------------------------------------------------------------
    else if (mode > 1.5f && mode < 2.5f)
    {
        // 刃のハイライト部分を検出し、白銀の閃光を付加
        float bladeHighlight = pow(saturate(lum), 3.0f);
        float rimGlow = smoothstep(0.6f, 0.95f, lum) * (1.2f + sin(time * 8.0f) * 0.4f);
        col.rgb += float3(0.9f, 0.95f, 1.2f) * (bladeHighlight + rimGlow);
    }
    // -------------------------------------------------------------
    // Mode 3: ガラス破片（エッジのプリズム分光・色収差ライティング）
    // -------------------------------------------------------------
    else if (mode > 2.5f && mode < 3.5f)
    {
        // 結晶のフチに動的な虹色スペクトルを合成
        float2 spectralUV = In.TexCoord + float2(sin(time * 4.0f) * 0.02f, cos(time * 4.0f) * 0.02f);
        float3 rainbow = float3(
            0.5f + 0.5f * cos(time * 3.0f + spectralUV.x * 6.28f),
            0.5f + 0.5f * cos(time * 3.0f + spectralUV.y * 6.28f + 2.09f),
            0.5f + 0.5f * cos(time * 3.0f + spectralUV.x * 6.28f + 4.18f)
        );
        float crystalEdge = smoothstep(0.35f, 0.9f, lum);
        col.rgb += rainbow * crystalEdge * 1.2f;
        col.rgb += float3(1.2f, 1.2f, 1.5f) * pow(saturate(lum), 4.0f); // 鋭いキラメキ
    }
    // -------------------------------------------------------------
    // Mode 4: 放射グレア（中心光条のエネルギーオーラ強調）
    // -------------------------------------------------------------
    else if (mode > 3.5f && mode < 4.5f)
    {
        float beamGlow = pow(saturate(lum), 1.8f);
        col.rgb *= float3(1.1f, 1.05f, 1.2f) * (1.0f + beamGlow * 1.6f);
        autoAlpha = saturate(lum * 1.5f) * In.Diffuse.a;
    }

    col.rgb *= In.Diffuse.rgb;
    outDiffuse.rgb = col.rgb;
    outDiffuse.a = autoAlpha;

    clip(outDiffuse.a - 0.01f);
}