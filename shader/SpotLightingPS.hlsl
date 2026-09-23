#include "Common.hlsl"

Texture2D g_Texture : register(t0); //0番テクスチャ
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    float spot = 0.0f;
    float light = 0.0f;
    float specular = 0.0f;
    float3 Vp = normalize((In.WorldPosition.xyz - Light.Position.xyz));
    float angle = acos(dot(Vp.xyz, Light.Direction.xyz));
    
    {
        spot = saturate(1.0f - pow(1.0f / Light.Angle.x * abs(angle), Light.PointLightParam.y));
        
        float4 normal = normalize(In.Normal);
        light = -dot(normal.xyz, Vp);
        float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
        eyev = normalize(eyev);
        float3 refv = reflect(Vp, normal.xyz);
        //float3 refv = reflect(Light.Direction.xyz, normal.xyz);
        refv = normalize(refv);
        specular = -dot(eyev, refv);
        saturate(specular);
        specular = pow(specular, 30.0f);
        float dist = length(In.WorldPosition.xyz - Light.Position.xyz);
        float ofs = saturate(1.0f - dist / Light.PointLightParam.x);
        spot *= ofs;
    }
    
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= Light.Diffuse.rgb * In.Diffuse.rgb * light * spot + Light.Ambient.rgb;
    outDiffuse.a *= In.Diffuse.a;
    outDiffuse.rgb += (specular * spot);
}