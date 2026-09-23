#include "Common.hlsl"

Texture2D g_Texture : register(t0); //0番テクスチャ
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    // 法線の正規化
    float4 normal = normalize(In.Normal);
    // 光源計算
    float light = -dot(normal.xyz, Light.Direction.xyz);
    //lightをサチュレート
    light = saturate(light);
    
    // テクセル取得
    outDiffuse.rgba = g_Texture.Sample(g_SamplerState, In.TexCoord);
    // テクセルの明るさを乗算する
    outDiffuse.rgb *= In.Diffuse.rgb * light;
    // α値は別処理
    outDiffuse.a = In.Diffuse.a;
    
    // カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    // 正規化する
    eyev = normalize(eyev);
    // 光の反射ベクトルを計算
    float3 refv = reflect(Light.Direction.xyz, normal.xyz);
    // 正規化する
    refv = normalize(refv);
    
    // スペキュラ成分
    float specular = -dot(eyev, refv);
    // 値をサチュレート
    specular = saturate(specular);
    // スペキュラーを30乗
    specular = pow(specular, 30);
    
    outDiffuse.rgb += specular;
}