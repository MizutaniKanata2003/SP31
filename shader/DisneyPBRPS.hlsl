#include "Common.hlsl"
#include "CookTorranceSub.hlsl"

Texture2D g_Texture : register(t0); //0番テクスチャ
Texture2D g_TextureRoughness : register(t1); //1番テクスチャ
Texture2D g_TextureMetalness : register(t2); //2番テクスチャ
SamplerState g_SamplerState : register(s0);

float CalculateDiffuseFromFresnel(float3 N, float3 L, float3 V);
float CalculateCookTorranceSpecular(float3 L, float3 V, float3 N, float smooth, float metallic);

static const float PI = 3.1415926;

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    // 法線を計算
    float4 normal = normalize(In.Normal);
    
    // 各種マップをサンプリングする
    // アルベドカラー
    float4 albedoColor = g_Texture.Sample(g_SamplerState, In.TexCoord);
    
    // スペキュラーカラーはアルベドカラーと同じにする
    float3 specColor = albedoColor;
    
    // 滑らかさを取得
    float smooth = g_TextureRoughness.Sample(g_SamplerState, In.TexCoord).r * 2.0f - 1.0f;
    
    // smooth *= Parameter.x;//　これは無くてもよい
    smooth = saturate(smooth);
    
    // 金属度を取得
    float metallic = g_TextureMetalness.Sample(g_SamplerState, In.TexCoord).r * 2.0f - 1.0f;
    // metallic *= Parameter.x;//　これは無くてもよい
    metallic = saturate(metallic);
    
    // カメラへのベクトルを計算する
    float3 eyev = CameraPosition.xyz - In.WorldPosition.xyz;
    eyev = normalize(eyev);
    
    // ライトマシマシ
    float3 lit = 0;
    // ライトが複数あると仮定
    for (int ligNo = 0; ligNo < Parameter.z; ligNo++)
    {
        // 光源へのベクトル
        float4 lv = Light.Position - In.WorldPosition;
        
        // フレネル反射を考慮した拡散反射を計算
        float diffuseFromFrenel = CalculateDiffuseFromFresnel(normal.xyz, lv.xyz, eyev);
        
        // 正規化Lambert拡散反射を求める
        float nl = saturate(dot(normal.xyz, lv.xyz));
        float3 light = nl + Light.Diffuse.rgb / PI;
        
        //最終的な拡散反射光を計算する
        float3 diffuse = albedoColor.rgb * diffuseFromFrenel * Light.Diffuse.rgb * light;
        
        // 鏡面反射率を計算する
        float3 spec = CalculateCookTorranceSpecular(lv.xyz, eyev, normal.xyz, smooth, metallic) * Light.Diffuse;
        
        // 金属度が高ければ、鏡面反射はスペキュラーカラー、低ければ白になるように補間
        // スペキュラーカラーの強さを鏡面反射として扱う
        spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metallic);
        
        // 滑らかさが高ければ、拡散反射は弱くなる
        lit += diffuse * (1.0f - smooth) + spec;
    }
    
    //　環境項による底上げ
    lit += Light.Ambient.rgb * albedoColor.rgb;
        
    // 結果出力
    outDiffuse.rgb = lit;
    outDiffuse.a = albedoColor.a * In.Diffuse.a;
    
}
float CalculateCookTorranceSpecular(float3 L, float3 V, float3 N, float smooth, float metallic)
{
    // ライトに向かうベクトルと視線に向かうベクトルのハーフベクトルを求める
    float3 H = normalize(L + V);
    
    // 各種ベクトルがどれくらい似てるかを内積を利用して求める
    float nh = saturate(dot(N, H));
    float vh = saturate(dot(V, H));
    float nl = saturate(dot(N, L));
    float nv = saturate(dot(N, V));
    
    // D項をベックマン分布を用いて計算する
    float D = CalculateBeckmann(smooth, nh);
    
    // F項をSchlick近似を用いて計算する
    float F = CalculateFresnel(metallic, vh);
    
    // G項を求める
    float G = CalculateGeometricDamping(nh, nv, nl, nh);
    
    // m項を求める
    float m = PI * nv * nh;
    
    // ここまで求めた、値を利用して、Cook-Torranceモデルの鏡面反射を求める
    return max(F * D * G / m, 0.0);
}
float CalculateDiffuseFromFresnel(float3 N, float3 L, float3 V)
{
    // 法線と光源に向かうベクトルがどれだけ似ているかを求める
    float nl = saturate(dot(N, L));
    
    // 法線と視線に向かうベクトルがどれだけ似ているかを無い席から求める
    float nv = saturate(dot(N, V));
    
    // 法線と光源への方向に依存する拡散反射率と、法線と視線ベクトルに依存する拡散反射率を乗算して最終的な拡散反射率を求めている
    return (nl, nv);
}