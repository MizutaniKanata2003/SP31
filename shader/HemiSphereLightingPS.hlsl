#include "Common.hlsl"

Texture2D g_Texture : register(t0); //0番テクスチャ
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    // 法線の正規化
    float4 normal = normalize(In.Normal);
    // 光源計算
    float light = -dot(normal.xyz, normalize(Light.Direction.xyz));
    // テクセル取得
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    // テクセルの明るさを乗算する
    outDiffuse.rgb *= In.Diffuse.rgb * light;
    outDiffuse.rgb *= Light.Ambient.rbg;
    outDiffuse.a = In.Diffuse.a;
    
    // カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    // 正規化する
    eyev = normalize(eyev);
    // ハーフベクトル
    float halfv = eyev + normalize(Light.Direction.xyz);
    halfv = normalize(halfv);
    
    // 鏡面反射計算
    float specular = -dot(eyev, normal.xyz);
    // 値をサチュレート
    specular = saturate(specular);
    // スペキュラーを30乗
    specular = pow(specular, 30);
    
    outDiffuse.rgb += specular;
    
    // 半球ライティング
    float norm = dot(normal, normalize(Light.GroundNormal));
    norm = (norm + 1.0f) / 2.0f;
    float3 hemiColor = lerp(+Light.GroundColor, Light.SkyColor, norm);
    outDiffuse.rgb += hemiColor;
}