//-------------------------------------------------------------------------------
//
// Gumshoe Framework v1.00
//   - Based on MJP's DX11 Sample Framework (http://mynameismjp.wordpress.com/)
//
//  All code licensed under the MIT license
//
//-------------------------------------------------------------------------------

#pragma once

#include "..\\PCH.h"

namespace GumshoeFramework10
{

enum LightType
{
    tDirectionalLight  = 0,
    tPointLight        = 1,
    tSpotLight         = 2,
    tAreaLight         = 3
};

// Note: Make sure structure alignment agrees with HLSL structure padding rules. 
//   Elements are packed into 4D vectors with the restriction that an element
//   cannot straddle a 4D vector boundary.

struct DirectionalLight
{
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Packed into 4D vector: (Position, Range)
	XMFLOAT3 Position;
	float Range;

	// Packed into 4D vector: (A0, A1, A2, Pad)
	XMFLOAT3 Att;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Packed into 4D vector: (Position, Falloff)
	XMFLOAT3 Position;
	float Falloff;

	// Packed into 4D vector: (Direction, Inner Spot Angle)
	XMFLOAT3 Direction;
	float InnerAngle;

	// Packed into 4D vector: (Att, Outer Spot Angle)
	XMFLOAT3 Att;
	float OuterAngle;

	// Packed into 4D vector: (Range, Pad[3])
	float Range;
	float Pad[3];
};

struct SimpleMaterial
{
	SimpleMaterial() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; // w = SpecPower
	XMFLOAT4 Reflect;
	XMFLOAT4 Emissive;
};

} // end of namespace GumshoeFramework10