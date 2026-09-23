#include "Common.hlsl"

Texture2D g_Texture : register(t0); //0番テクスチャ
SamplerState g_SamplerState : register(s0); // サンプラー0番

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    // 光源からピクセルへのベクトル
    float4 lv = Light.Position - In.WorldPosition;
    // 物体と光源の距離
    float4 ld = length(lv);
    // ベクトルの正規化
    lv = normalize(lv);
    
    // 減衰の計算
    float ofs = 1.0f - (1.0f / Light.PointLightParam.x) * ld;
    // 減衰率0未満は9にする
    ofs = max(0, ofs);
    
    // ピクセルの法線を正規化
    float4 normal = normalize(In.Normal);
    // 光源計算
    float light = 0.5f + 0.5f * dot(normal.xyz, lv.xyz);
    light = saturate(light);
    
    // light <= 明るさの調整(段階分け)
    if (light > Parameter.y)
    {
        light = 1.0f;
    }
    else if (light.x > Parameter.x)
    {
        light = 0.7f;
    }
    else
    {
        light = 0.4f;
    }
    // 明るさを減衰
    light *= ofs;
    
    // テクスチャのピクセル色を取得
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    // 明るさを乗算
    outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light + Light.Ambient.rgb;
    // α値に明るさは関係ない
    outDiffuse.a * In.Diffuse.a;
    
    // 簡易エッジを作成
    float4 eyev = In.WorldPosition - CameraPosition;
    eyev = normalize(eyev);
    
    float d = dot(normal, eyev);
    if (d > -0.3f)
    {
        //明度を落とす
        outDiffuse.rgb *= 0.3f;
    }
}