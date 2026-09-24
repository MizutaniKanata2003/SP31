
#include "Common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    // WVP行列を作成
    matrix wvp; // World * View * Projection
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp);
    Out.Normal = In.Normal;
    Out.Diffuse = In.Diffuse;
   
    // 正規化デバイス座標を計算する
    float3 tex;
    tex.x = Out.Position.x / Out.Position.w;
    tex.y = Out.Position.y / Out.Position.w;
    //{
    //    tex.x = (tex.x * 0.5f) + 0.5f; // テクスチャ座標へ変換
    //    tex.y = (-tex.y * 0.5f) + 0.5f;
        
    //    tex.x += Parameter.x;
    //    tex.y += Parameter.x;
    //}
    {
        // 法線利用版
        float4 worldNormal, normal;
        normal = float4(In.Normal.xyz, 0.0);
        worldNormal = mul(normal, World);
        worldNormal = normalize(worldNormal);
        
        tex.x = (tex.x * 0.5f) + 0.5f;
        tex.y = (-tex.y * 0.5f) + 0.5f;
        float3 vnorm = mul(worldNormal, View);
        vnorm = normalize(vnorm);
        tex.x += (vnorm.x * Parameter.x);
        tex.y += (vnorm.y * Parameter.x);
    }
    Out.TexCoord = tex;
}
