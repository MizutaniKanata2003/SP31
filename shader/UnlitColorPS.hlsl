
#include "Common.hlsl"

Texture2D g_Texture : register(t0);	//0番テクスチャ
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
	outDiffuse = In.Diffuse;//ピクセルの色を格納
	
	//テクセルの色を乗算して出力
    outDiffuse *= g_Texture.Sample(g_SamplerState, In.TexCoord);
}



