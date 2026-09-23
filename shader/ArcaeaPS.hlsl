#include "Common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    float4 col = g_Texture.Sample(g_SamplerState, In.TexCoord);
    
    // 頂点カラー（Diffuse）の乗算
    col *= In.Diffuse;

    // 黒背景の輝度を自動検出してアルファに変換（黒抜きブレンド）
    float luminance = max(col.r, max(col.g, col.b));
    float autoAlpha = saturate(luminance * 2.5f) * In.Diffuse.a;

    // 円形ソフトフェード（四角いフチの境目を消す）
    float2 uvCenter = In.TexCoord - float2(0.5f, 0.5f);
    float dist = length(uvCenter);
    float edgeMask = smoothstep(0.5f, 0.45f, dist);

    // Parameter.x == 1.0 のときは円形マスクを適用
    if (Parameter.x > 0.5f)
    {
        autoAlpha *= edgeMask;
    }

    outDiffuse.rgb = col.rgb;
    outDiffuse.a = autoAlpha;

    // アルファがほぼ0ならピクセル破棄
    clip(outDiffuse.a - 0.01f);
}