//-------------------------------------------------------------------------------
//
// PBR Mesh shader for LightingDemo
//   - Using GumshoeFramework (v1.00)
//
//  All code licensed under the MIT license
//
//-------------------------------------------------------------------------------

#include "AppSettings.hlsl"
#include "MeshHelperPbr.hlsl"
 
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

//=================================================================================================
// Vertex Shader
//=================================================================================================
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


// ================================================================================================
// Calculates the Fresnel factor using Schlick's approximation
// ================================================================================================
float3 Fresnel(in float3 specAlbedo, in float3 h, in float3 l)
{
    float3 fresnel = specAlbedo + (1.0f - specAlbedo) * pow((1.0f - saturate(dot(l, h))), 5.0f);

    // Fade out spec entirely when lower than 0.1% albedo
    fresnel *= saturate(dot(specAlbedo, 333.0f));

    return fresnel;
}

// ===============================================================================================
// Helper for computing the GGX visibility term
// ===============================================================================================
float GGX_V1(in float m2, in float nDotX)
{
    return 1.0f / (nDotX + sqrt(m2 + (1 - m2) * nDotX * nDotX));
}

// ===============================================================================================
// Computes the specular term using a GGX microfacet distribution, with a matching
// geometry factor and visibility term. Based on "Microfacet Models for Refraction Through
// Rough Surfaces" [Walter 07]. m is roughness, n is the surface normal, h is the half vector,
// l is the direction to the light source, and specAlbedo is the RGB specular albedo
// ===============================================================================================
float GGX_Specular(in float m, in float3 n, in float3 h, in float3 v, in float3 l)
{
    float nDotH = saturate(dot(n, h));
    float nDotL = saturate(dot(n, l));
    float nDotV = saturate(dot(n, v));

    float nDotH2 = nDotH * nDotH;
    float m2 = m * m;

    // Calculate the distribution term
    float d = m2 / (Pi * pow(nDotH * nDotH * (m2 - 1) + 1, 2.0f));

    // Calculate the matching visibility term
    float v1i = GGX_V1(m2, nDotL);
    float v1o = GGX_V1(m2, nDotV);
    float vis = v1i * v1o;

    return d * vis;
}

float3 CalcLighting(in float3 normal, in float3 lightDir, in float3 lightColor,
					in float3 diffuseAlbedo, in float3 specularAlbedo, in float roughness,
					in float3 positionWS)
{
    float3 lighting = diffuseAlbedo * (1.0f / 3.14159f);

    float3 view = normalize(CameraPosWS - positionWS);
    const float nDotL = saturate(dot(normal, lightDir));
    if(nDotL > 0.0f)
    {
        const float nDotV = saturate(dot(normal, view));
        float3 h = normalize(view + lightDir);

        float specular = GGX_Specular(roughness, normal, h, view, lightDir);
        float3 fresnel = Fresnel(specularAlbedo, h, lightDir);

        lighting += specular * fresnel;
    }

    return lighting * nDotL * lightColor;
}

//=================================================================================================
// Pixel Shader
//=================================================================================================
float4 PS(VertexOut pin) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
    float3 normalWS = normalize(pin.NormalW);
    float3 viewWS = normalize(gEyePosW - pin.PosW);
    float3 positionWS = pin.PosW;

    float4 albedoMap = gMaterial.Diffuse;

    float3 diffuseAlbedo = albedoMap.xyz;
    diffuseAlbedo *= DiffuseIntensity;
    diffuseAlbedo *= (1.0f - SpecularIntensity);

    // Add in the primary directional light
    //float shadowVisibility = EnableShadows ? ShadowVisibility(positionWS, input.DepthVS) : 1.0f;
    float shadowVisibility = 1.0f;
    
    float3 lighting = 0.0f;

    if(EnableDirectionalLight)
        lighting += CalcLighting(normalWS, DirLightDirection, DirLightColor, diffuseAlbedo, SpecularIntensity,
                                 Roughness, positionWS) * shadowVisibility;

	// Add in the ambient
    if(EnableAmbientLighting)
    {
        float3 indirectDiffuse = EvalSH9Cosine(normalWS, EnvironmentSH);

        lighting += indirectDiffuse * diffuseAlbedo;

        float3 reflectWS = reflect(-viewWS, normalWS);
        float3 vtxReflectWS = reflect(-viewWS, vtxNormal);

        uint width, height, numMips;
        SpecularCubemap.GetDimensions(0, width, height, numMips);

        const float SqrtRoughness = sqrt(Roughness);

        // Compute the mip level, assuming the top level is a roughness of 0.01
        float mipLevel = saturate(SqrtRoughness - 0.01f) * (numMips - 1.0f);

        float gradientMipLevel = SpecularCubemap.CalculateLevelOfDetail(LinearSampler, vtxReflectWS);
        if(UseGradientMipLevel)
            mipLevel = max(mipLevel, gradientMipLevel);

        // Compute fresnel
        float viewAngle = saturate(dot(viewWS, normalWS));
        float2 AB = SpecularCubemapLookup.SampleLevel(LinearSampler, float2(viewAngle, SqrtRoughness), 0.0f);
        float fresnel = SpecularIntensity * AB.x + AB.y;
        fresnel *= saturate(SpecularIntensity * 100.0f);

        lighting += SpecularCubemap.SampleLevel(LinearSampler, reflectWS, mipLevel) * fresnel;
    }

    PSOutput output;
    output.Color = float4(lighting, 1.0f);

    output.Color.xyz *= exp2(ExposureScale);

    float2 prevPositionSS = (input.PrevPosition.xy / input.PrevPosition.z) * float2(0.5f, -0.5f) + 0.5f;
    prevPositionSS *= RTSize;
    output.Velocity = input.PositionSS.xy - prevPositionSS;
    output.Velocity -= JitterOffset;

    return output;
}