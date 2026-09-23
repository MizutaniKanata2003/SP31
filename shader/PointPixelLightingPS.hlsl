#include "Common.hlsl"

Texture2D g_Texture : register(t0); //0番テクスチャ
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
 //ベクトル計算
 //==============================================================
    // 光源からピクセルへのベクトル
    float4 lv = In.WorldPosition - Light.Position;
    // 物体と光源の距離
    float4 ld = length(lv);
    //　ベクトルの正規化
    lv = normalize(lv);
    // 減衰の計算
    float offSet = saturate(1.0f - ld / Light.PointLightParam.x);
    // 減衰率0未満は0にする
    offSet = max(0.0f, offSet);
    // 法線の正規化
    float4 normal = normalize(In.Normal);
 //==============================================================
    
    // 光源計算
    float light = -dot(normal.xyz, normalize(Light.Direction.xyz));
    // lightをサチュレート
    light = saturate(light);
    // 明るさを減衰する
    light *= offSet;
    // テクセル取得
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    // テクセルの明るさを乗算する
    outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rbg * light + Light.Ambient.agb;
    outDiffuse.a = In.Diffuse.a;
    
    
 //==============================================================
    // スペキュラー計算
    
    // カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    // 正規化する
    eyev = normalize(eyev);
    // ハーフベクトル
    float halfv = eyev + lv.xyz;
    halfv = normalize(halfv);
    
    // 鏡面反射計算
    float specular = -dot(eyev, normal.xyz);
    // 値をサチュレート
    specular = saturate(specular);
    // スペキュラーを30乗
    specular = pow(specular, 30);
    
    outDiffuse.rgb += specular;
 //==============================================================
}