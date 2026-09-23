#include "Common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    matrix wvp; // World * View * Projection
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(In.Position, wvp);
    
    float4 worldNormal, normal;
    
    normal = float4(In.Normal.xyz, 0.0); // wの要素は0にする
    
    worldNormal = mul(normal, World); // 法線回転
    worldNormal = normalize(worldNormal); // 正規化
    Out.Normal = worldNormal; // 法線出力
    
    // 明るさ計算 光ベクトルと法線の内積計算
    float light = -dot(Light.Direction.xyz, worldNormal.xyz);
    light = saturate(light); // 明るさを0.0～1.0にする
    // lihgtに頂点の明るさができる
    
    // 明るさの出力
    Out.Diffuse.rgb = light; // 明るさは色データとなる
    // α値出力
    Out.Diffuse.a = In.Diffuse.a; // aは頂点カラーを使う
    // テクスチャ座標の出力
    Out.TexCoord = In.TexCoord;
}