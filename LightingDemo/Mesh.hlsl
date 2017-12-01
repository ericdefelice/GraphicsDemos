//-------------------------------------------------------------------------------
//
// Basic Mesh shader for LightingDemo
//   - Using GumshoeFramework (v1.00)
//
//  All code licensed under the MIT license
//
//-------------------------------------------------------------------------------

#include "AppSettings.hlsl"
#include "MeshHelper.hlsl"
 
cbuffer cbPerFrame : register(b0)
{
    DirectionalLight gDirLight;
    PointLight gPointLight;
    SpotLight gSpotLight;
    float3 gEyePosW;
};

cbuffer cbPerObject : register(b1)
{
    float4x4 gWorld;
    float4x4 gView;
    float4x4 gWorldViewProj;
    Material gMaterial;
};

struct VertexIn
{
    float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
};

struct VertexOut
{
    float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    
    // Transform to world space space.
    vout.PosW    = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
    vout.NormalW = normalize(mul(vin.NormalL, (float3x3)gWorld));
        
    // Transform to homogeneous clip space.
    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
    
    return vout;
}
  
float4 PS(VertexOut pin) : SV_Target
{
    // Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW); 

    float3 toEyeW = normalize(gEyePosW - pin.PosW);

    // Start with a sum of zero.
    float4 emissive = gMaterial.Emissive;
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Sum the light contribution from each light source.
    float4 A, D, S;

    ComputeDirectionalLight(gMaterial, gDirLight, pin.NormalW, toEyeW, A, D, S);
    ambient += A;  
    diffuse += D;
    spec    += S;

    if (EnablePointLight)
    {
      ComputePointLight(gMaterial, gPointLight, pin.PosW, pin.NormalW, toEyeW, D, S);
      diffuse += D;
      spec    += S;
    }

    if (EnableSpotLight)
    {
      ComputeSpotLight(gMaterial, gSpotLight, pin.PosW, pin.NormalW, toEyeW, D, S);
      diffuse += D;
      spec    += S;
    }
       
    float4 litColor = max(emissive, ambient + diffuse + spec);
    
    // Common to take alpha from diffuse material.
    litColor.a = max(gMaterial.Diffuse.a, gMaterial.Emissive.a);

    return litColor;
}

