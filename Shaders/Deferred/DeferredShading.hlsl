//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

//Handles sampling from GBuffer for lighting

#ifndef DEFERRED_SHADING_HLSL
#define DEFERRED_SHADING_HLSL

#include "Utils/Defines.hlsl"
#include "Utils/ConstantBuffers.hlsl"
#include "Lighting/Lights.hlsl"

Texture2D<float4> DiffuseTexture   : register(t0);
Texture2D<float3> NormalTexture    : register(t1);
Texture2D<float4> SpecularTexture  : register(t2);//rgb specular color, a roughness
Texture2D<float4> EmissiveTexture  : register(t3);//A uses for metallic; significant waste of bits
Texture2D<float>  DepthTexture     : register(t4);

TextureCube       EnvironmentProbe : register(t5);

RWTexture2D<float4> OutputTexture  : register(u0);

SamplerState PointSampler          : register(s0);
SamplerState EnvironmentSampler    : register(s1);

StructuredBuffer<PointLight> gPointLights : register(t6);

struct SURFACE_DATA
{
	float3 PositionView;
	float3 PositionWorld;
	float4 Diffuse;
	float3 Normal;
	float3 SpecularColor;
	float Roughness;
	float Metallic;
	float3 Emissive;
};

//http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float RadicalInverse_VdC(uint bits) {
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

float2 Hammersley(uint i, uint N) {
	return float2(float(i) / float(N), RadicalInverse_VdC(i));
}

float D_GGX(float a, float NoH)
{
	float a2 = a * a;
	float denom = (NoH * NoH) * (a2 - 1.0) + 1.0;
	denom *= denom;

	return a2 / (PI * denom);
}

float G1_Smith(float k, float NoV)
{
	return NoV / (NoV * (1.0 - k) + k);
}

float G_Smith(float r, float NoV, float NoL)
{
	/*float a = (r * r);
	float Vis_SmithV = NoL * (NoV * (1 - a) + a);
	float Vis_SmithL = NoV * (NoL * (1 - a) + a);
	return 0.5 * rcp(Vis_SmithV + Vis_SmithL);*/

	/*float a = r*r;
	float a2 = a*a;
	float Vis_SmithV = NoV + sqrt(NoV * (NoV - NoV * a2) + a2);
	float Vis_SmithL = NoL + sqrt(NoL * (NoL - NoL * a2) + a2);
	return rcp(Vis_SmithV * Vis_SmithL);*/

	float r2 = (r + 1) * (r + 1);
	float k = r2 / 8.0;
	return G1_Smith(k, NoV) * G1_Smith(k, NoL);
}

//F0 is specular color at normal incidence
float3 F_Schlick(float3 f0, float VoH)
{
	return f0 + (1.0 - f0) * exp2((-5.55473 * VoH) - (6.98316 * VoH));

	//float Fc = pow(1 - VoH, 5);
	//return saturate(50.0 * f0) * Fc + (1 - Fc) * f0;
}


//http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
//Used for getting a normal to sample a cube map given a sine and cosine, roughness, and base world space normal
float3 ImportanceSampleGGX(float2 Xi, float Roughness, float3 N)
{
	float a = Roughness * Roughness;
	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt((1 - Xi.y) / (1 + (a*a - 1) * Xi.y));
	float SinTheta = sqrt(1 - CosTheta * CosTheta);

	float3 H;
	H.x = SinTheta * cos(Phi);
	H.y = SinTheta * sin(Phi);
	H.z = CosTheta;

	float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 TangentX = normalize(cross(UpVector, N));
	float3 TangentY = cross(N, TangentX);
	// Tangent to world space
	return TangentX * H.x + TangentY * H.y + N * H.z;
}

float3 SpecularIBL(float3 SpecularColor, float Roughness, float3 N, float3 V)
{
	float3 SpecularLighting = 0;
	const uint NumSamples = 128;

	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 H = ImportanceSampleGGX(Xi, Roughness, N);
		float3 L = 2 * dot(V, H) * H - V;

		float NoV = abs(dot(N, V));
		float NoL = saturate(dot(N, L));
		float NoH = saturate(dot(N, H));
		float VoH = saturate(dot(V, H));

		if (NoL > 0)
		{
			float3 SampleColor = EnvironmentProbe.SampleLevel(EnvironmentSampler, L, 0).rgb;
			float G = G_Smith(Roughness, NoV, NoL);
			float Fc = pow(1 - VoH, 5);
			float3 F = (1 - Fc) * SpecularColor + Fc;
			// Incident light = SampleColor * NoL
			// Microfacet specular = D*G*F / (4*NoL*NoV)
			// pdf = D * NoH / (4 * VoH)
			SpecularLighting += SampleColor * F * G * VoH / (NoH * NoV);
		}
	}

	return SpecularLighting / NumSamples;
}

void AccumulateBRDF(SURFACE_DATA surface, PointLight light, float3 toEye, inout float3 finalColor)//Need to optimize
{
	float roughness2 = surface.Roughness * surface.Roughness;

	float3 toLight = light.PositionWorld - surface.PositionWorld;
	float distance = length(toLight);
	toLight /= distance;

	float3 H = normalize(toLight + toEye);

	float NoV = abs(dot(surface.Normal, toEye));
	float NoL = saturate(dot(surface.Normal, toLight));
	float NoH = saturate(dot(surface.Normal, H));
	float VoH = saturate(dot(toEye, H));
	float LoH = saturate(dot(toLight, H));

	float distanceSq = distance * distance;
	float oneOverDistSq = rcp(distanceSq);

	float D = D_GGX(roughness2, NoH);
	float G = G_Smith(surface.Roughness, NoV, NoL);
	float3 F = F_Schlick(lerp(float3(0.0f, 0.0f, 0.0f), surface.SpecularColor, surface.Metallic), VoH);

	float3 contribution = (D*G*F) / (4*NoL*NoV);

	finalColor += light.Color / PI * surface.Diffuse * light.Intensity * saturate(NoL) * oneOverDistSq * (1.0f - surface.Metallic);//Diffuse
	finalColor += light.Color * light.Intensity * saturate(contribution) * oneOverDistSq;//Specular
}

float ConvertZToLinearDepth(float depth)
{
	float linearDepth = gProjection._34 / (depth + gProjection._33);
	return linearDepth;
}

float3 GetViewPos(float2 posClip, float viewSpaceZ)
{
	float3 position;
	//float linearDepth = ConvertZToLinearDepth(depth);

	position.xy = posClip.xy * float2(1.0 / gProjection._11, 1.0 / gProjection._22) * viewSpaceZ;
	position.z = viewSpaceZ;

	return position;
}

float3 GetWorldPos(float2 posClip, float depth)
{
	float4 position;

	position.xy = posClip.xy;

	position.z = depth;
	position.w = 1.0;


	position = mul(gViewProjInv, position);

	return position.xyz / position.w;
}

//SURFACE_DATA UnpackGBuffer(float2 UV)
//{
//	SURFACE_DATA Out;
//
//	Out.Depth = DepthTexture.Sample(PointSampler, UV).r;
//	Out.Diffuse = DiffuseTexture.Sample(PointSampler, UV);
//	Out.Normal = normalize(NormalTexture.Sample(PointSampler, UV).rgb * 2.0 - 1.0);
//	float4 spec = SpecularTexture.Sample(PointSampler, UV);
//	Out.SpecularColor = spec.rgb;
//	Out.Roughness = spec.a;
//	Out.Emissive = EmissiveTexture.Sample(PointSampler, UV).rgb;
//
//	return Out;
//}

SURFACE_DATA UnpackGBufferViewport(uint2 viewportPosition)
{
	SURFACE_DATA Out;

	
	Out.Diffuse = DiffuseTexture.Load(uint3(viewportPosition, 0));
	Out.Normal = NormalTexture.Load(uint3(viewportPosition, 0)).rgb * 2.0 - 1.0;
	float4 spec = SpecularTexture.Load(uint3(viewportPosition, 0));
	Out.SpecularColor = spec.rgb;
	Out.Roughness = spec.a;
	float4 emissiveMetallic = EmissiveTexture.Load(uint3(viewportPosition, 0));
	Out.Emissive = emissiveMetallic.rgb;
	Out.Metallic = emissiveMetallic.a;

	float2 GBufferDim;
	DiffuseTexture.GetDimensions(GBufferDim.x, GBufferDim.y);

	//Calcluate screen space position
	float2 ScreenPixelOffset = float2(2.0f, -2.0f) / GBufferDim;
	float2 ScreenPosition = (float2(viewportPosition.xy) + 0.5f) * ScreenPixelOffset.xy + float2(-1.0f, 1.0f);

	float zBuffer = DepthTexture.Load(uint3(viewportPosition, 0)).r;
	float ViewSpaceZ = ConvertZToLinearDepth(zBuffer);

	Out.PositionView = GetViewPos(ScreenPosition, ViewSpaceZ);
	Out.PositionWorld = GetWorldPos(ScreenPosition, zBuffer);

	return Out;
}

#endif