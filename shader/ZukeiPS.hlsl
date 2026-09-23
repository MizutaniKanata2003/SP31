
#include "Common.hlsl"

Texture2D g_Texture : register(t0); //0î‘ÉeÉNÉXÉ`ÉÉ
SamplerState g_SamplerState : register(s0);


// ìhÇËÇ¬Ç‘ÇµÇÃâ~
float circle(float2 tex, float rad)
{
    return length(tex) - rad;
}

float ring(float2 tex, float rad, float w)
{
    w = rad - w;
    return abs(length(tex) - rad) + w;
}

float heart(float2 tex, float size)
{
    float hekomi = Parameter.w;
    tex.x = 2.0f * tex.x - sign(tex.x) * (-tex.y) * hekomi;
    return length(tex) - size;
}
void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    float ASPECT = (1280.0f / 720.0f);
    // ìhÇËÇ¬Ç‘ÇµÇÃâ~
    //{
    //    float loop = 1.0f;
    //    float2 tex = frac(In.TexCoord * loop) * 2.0f - 1.0f;
    //    tex.x *= ASPECT;
        
    //    float rad = Parameter.x;
    //    float w = Parameter.w;
        
    //    float col = ring(tex, rad, w);
    //    //float col = circle(tex, rad);
        
    //    col = smoothstep(rad, rad + Parameter.y, col);
        
    //    outDiffuse.rgb = 1.0f - col;
    //    outDiffuse.a = 1.0f;
    //}
    // ÉnÅ[Ég
      {
        float loop = 1.0f;
        float2 tex = frac(In.TexCoord * loop) * 2.0f - 1.0f;
        
        float size = Parameter.x;
        float col = heart(tex, size);
        
        col = smoothstep(size, size + Parameter.y, col);
        
        outDiffuse.rgb = 1.0f - col;
        outDiffuse.a = 1.0f;
    }
}



