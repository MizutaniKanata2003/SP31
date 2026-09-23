#include "Common.hlsl"
#include "CookTorranceSub.hlsl"

Texture2D g_Texture : register(t0); //0番テクスチャ
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    // ぷくセルから光源へのベクトル(いつもと逆)
    float4 lv = Light.Position - In.WorldPosition;
    // 物体と光源の距離
    float4 ld = length(lv);
    // ベクトルの正規化
    lv = normalize(lv);
    // 減衰の計算
    float ofs = 1.0f - (1.0f / Light.PointLightParam.x) * ld;
    // 減衰率0未満は0にする。
    ofs = max(0, ofs);
    
    // ピクセルの法線を正規化
    float4 normal = normalize(In.Normal);
    // 光源計算
    float light = 0.5f + 0.5f * dot(normal.xyz, lv.xyz);
    light = saturate(light);
    light *= ofs; // 明るさを減衰させる
    
    //　テクスチャのピクセル色を取得
    float4 albedocolor = g_Texture.Sample(g_SamplerState, In.TexCoord); // テクスチャ
    outDiffuse = albedocolor;
    outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light + Light.Ambient.rgb; //明るさを乗算
    outDiffuse.a *= In.Diffuse.a;
    
    // ピクセルからカメラへ向かうベクトル（いつもと逆）
    float3 eyev = CameraPosition.xyz - In.WorldPosition.xyz;
    eyev = normalize(eyev);
    
    // ハーフベクトルを計算
    float3 halfv = eyev + lv.xyz; // 視線ベクトル＋ライトベクトル
    halfv = normalize(halfv);
    
    //　スペキュラー計算
    float specular;
    float nv = saturate(dot(normal.xyz, eyev));
    float nh = saturate(dot(normal.xyz, halfv));
    float vh = saturate(dot(eyev, halfv));
    float n1 = saturate(dot(normal.xyz, lv.xyz));
    
    // 今回はテクスチャからの情報でなく設定した数値で代用
    float smooth = saturate(Parameter.x);
    float metallic = saturate(Parameter.y);
    
    float D = CalculateBeckmann(smooth, nh); //面の粗さ
    float G = CalculateGeometricDamping(nh, nv, n1, vh);
    float F = CalculateFresnel(metallic, dot(lv.xyz, halfv));
    
    specular = max(0.0f, F * D * G / nv) * albedocolor.rgb;
    
    outDiffuse.rgb += (specular * ofs); // = のみにしてスペキュラーだけにすると分かりやすい
}