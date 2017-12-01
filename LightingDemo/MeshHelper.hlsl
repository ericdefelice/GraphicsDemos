//-------------------------------------------------------------------------------
//
// Basic Mesh shader helper (performs lighting) for LightingDemo
//   - Using GumshoeFramework (v1.00)
//
//  All code licensed under the MIT license
//
//-------------------------------------------------------------------------------

struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float pad;
};

struct PointLight
{ 
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Att;
	float pad;
};

struct SpotLight
{
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Falloff;

	float3 Direction;
	float InnerAngle;

	float3 Att;
	float OuterAngle;

	float Range;
	float3 Pad;
};

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular; // w = SpecPower
	float4 Reflect;
	float4 Emissive;
};


//---------------------------------------------------------------------------------------
// Computes the attenuation for a SpotLight with min_angle, max_angle and the current
// light angle.  Returns a value in the range [0, 1] for the domain [min_angle, max_angle]
//---------------------------------------------------------------------------------------
float spotLightInterp(float min_angle, float max_angle, float curr_angle, float falloff)
{
    float t = saturate((curr_angle - min_angle)/(max_angle - min_angle));
    
    return pow(t, falloff);
    //return t*t*(3.0 - (2.0*t));
}


//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a directional light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputeDirectionalLight(Material mat, DirectionalLight L, 
                             float3 normal, float3 toEye,
					         out float4 ambient,
						     out float4 diffuse,
						     out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -L.Direction;

	// Add ambient term.
	if (EnableAmbientLighting)
	    ambient = mat.Ambient * L.Ambient;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	if (EnableDirectionalLight)
	{
		float diffuseFactor = dot(lightVec, normal);

		// Flatten to avoid dynamic branching.
		[flatten]
		if( diffuseFactor > 0.0f )
		{
			float3 v         = reflect(-lightVec, normal);
			float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
						
			diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
			spec    = specFactor * mat.Specular * L.Specular;
		}
	}
}

//---------------------------------------------------------------------------------------
// Computes the ambient, diffuse, and specular terms in the lighting equation
// from a point light.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputePointLight(Material mat, PointLight L, float3 pos, float3 normal, float3 toEye,
				       out float4 diffuse, out float4 spec)
{
	// Initialize outputs.
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	// Range test.
	if( d > (L.Range*2.0f) )
	  return;
		
	// Normalize the light vector.
	lightVec /= d; 
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if( diffuseFactor > 0.0f )
	{
		float3 v         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
					
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec    = specFactor * mat.Specular * L.Specular;
	}

	// Attenuate
	//float att = 1.0f / dot(L.Att, float3(1.0f, d, d*d));
	float falloff = saturate(1.0f - pow(d/L.Range, 4));
	float att = (falloff*falloff) / (d*d + 1.0f);
	
	diffuse *= att;
	spec    *= att;
}

//---------------------------------------------------------------------------------------
// Computes the diffuse, and specular terms in the lighting equation
// from a spotlight.  We need to output the terms separately because
// later we will modify the individual terms.
//---------------------------------------------------------------------------------------
void ComputeSpotLight(Material mat, SpotLight L, float3 pos, float3 normal, float3 toEye,
				      out float4 diffuse, out float4 spec)
{
	// Initialize outputs.
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	
    // The vector from the surface to the light.
	float3 lightVec = L.Position - pos;
		
	// The distance from surface to light.
	float dist = length(lightVec);
	
	// Normalize the light vector.
	lightVec /= dist;

    // Determine the angle between the current sample
    // and the light's direction
    float cosAlpha = max( 0.0f, dot( lightVec, -L.Direction ) );
    float cosSpotMinAngle = cos(L.InnerAngle);
    float cosSpotMaxAngle = cos(L.OuterAngle);

    // Compute the spot attenuation factor
    float fSpotAtten = 1.0f - spotLightInterp(cosSpotMinAngle, cosSpotMaxAngle, cosAlpha, L.Falloff);
    
    // Compute the attenuation factor for the light
    // float fAtt = 1.0f / dot(L.Att, float3(1.0f, dist, dist*dist));
    // Attenuate
	//float att = 1.0f / dot(L.Att, float3(1.0f, d, d*d));
	float falloff = saturate(1.0f - pow(dist/L.Range, 4));
	float fAtt = (falloff*falloff) / (dist*dist + 1.0f);

    // The final attenuation is the product of both types 
    // previously evaluated:
    float fAtten = fAtt * fSpotAtten;
 
    // Determine the diffuse color
    float NdotL = max( 0.0f, dot(lightVec, normal) );
    diffuse = fAtten * NdotL * L.Diffuse * mat.Diffuse;

    // Determine the specular color
    float3 R = normalize( reflect(-lightVec, normal) );
    float RdotV = max (0.0f, dot(R, toEye));
    float specFactor = pow(RdotV, mat.Specular.w);
    spec = fAtten * specFactor * L.Specular * mat.Specular;

/* 
 	// The vector from the surface to the light.
	float3 lightVec = L.Position - pos;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	// Range test.
	// if( d > L.Range )
	//   return;
		
	// Normalize the light vector.
	lightVec /= d; 
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
    float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if( diffuseFactor > 0.0f )
	{
		float3 v         = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
					
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
		spec    = specFactor * mat.Specular * L.Specular;
	}
	
	// Scale by spotlight factor and attenuate.
	float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);

	// Scale by spotlight factor and attenuate.
	float att = spot / dot(L.Att, float3(1.0f, d, d*d));

	diffuse *= att;
	spec    *= att;
*/

}

 
 