

#include "Common.hlsl"

Texture2D g_Texture : register(t0); //0番テクスチャ
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
     //ピクセルの色を格納
    outDiffuse = In.Diffuse;
	//テクセルの色を乗算して出力
    outDiffuse *= g_Texture.Sample(g_SamplerState, In.TexCoord);
    
    //グレースケールの計算
    float Y;
    Y = outDiffuse.r * 0.299f + outDiffuse.g * 0.587f + 
                            outDiffuse.b * 0.114f;
    outDiffuse.rgb = Y; //Aは変更しちゃダメ

    //セピア変換
    float4 sepia = float4(0.96f, 0.784f, 0.580f, 1.0f);
    outDiffuse.rgba *= sepia.rgba;
    
}







/*
    outDiffuse.rgb = outDiffuse.rgb * float3(0.299f, 0.587f, 0.114f);
    outDiffuse.rgb = outDiffuse.r + outDiffuse.g + outDiffuse.b;
 

*/




