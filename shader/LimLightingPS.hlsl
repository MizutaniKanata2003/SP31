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
    float light = -dot(normal.xyz, Light.Direction.xyz);
    // lightをサチュレート
    light = saturate(light);
    // 明るさを減衰する
    light *= offSet;

    // テクセルの明るさを乗算する
    outDiffuse.rgba = g_Texture.Sample(g_SamplerState, In.TexCoord); //テクスチャの色を取得
    outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light + Light.Ambient.rgb; //テクスチャの色に明るさを合成
    outDiffuse.a *= In.Diffuse.a; //α値は別処理
    
    
 //==============================================================
    // スペキュラー計算
    
    // カメラからピクセルへ向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    // 正規化する
    eyev = normalize(eyev);
    // ハーフベクトル
    float3 halfv = eyev + Light.Direction.xyz;
    halfv = normalize(Light.Direction.xyz + eyev);
    
    // 鏡面反射計算
    float specular = -dot(halfv, normal.xyz);
    // 値をサチュレート
    specular = saturate(specular);
    // スペキュラーを30乗
    specular = pow(specular, 30);
    
    outDiffuse.rgb += (specular * offSet);
 //==============================================================
    
    // リムライティング処理
    
    // 光の方向と視線ベクトルの考慮(逆光ほど明るい)
    // 光の方向ベクトルと視線ベクトルの最大値を取る(どちらも逆光ほど1.0になる)
    float lit = 1.0f - max(lv.xyz, eyev);
    // 輪郭部分ほど明るくする(直角に近いほど明るい)
    float lim = 1.0f - max(0.0f, dot(normal.xyz, -eyev)); // 法線と視線ベクトルの内積を取る（直角に近いほど0.0になる）
    // litとlimの明るさ合成
    // 逆光ほど明るく、輪郭部分ほど明るくする
    lim *= lit;
    //適当に調整
    lim = pow(lim, Light.PointLightParam.y);
    // リムライティングの明るさをディヒューズに加算
    outDiffuse.rgb += lim;
}