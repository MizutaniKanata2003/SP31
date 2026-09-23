#include "Common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    matrix wvp; // World * View * Projection
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp);
    
    float4 worldNormal, normal;
    
    normal = float4(In.Normal.xyz, 0.0f); // wの要素は0にする
    
    worldNormal = mul(normal, World); // 法線回転
    worldNormal = normalize(worldNormal); // 正規化
    Out.Normal = worldNormal; // 法線出力
   
    // α値出力
    Out.Diffuse = In.Diffuse;
    // テクスチャ座標の出力
    Out.TexCoord = In.TexCoord;
    
    Out.WorldPosition = mul(In.Position, World); // ワールド座標出力
}