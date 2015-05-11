//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#ifndef SHADING_HLSL
#define SHADING_HLSL

#include "Utils/Defines.hlsl"

struct SURFACE_DATA
{
	float2 PositionTextureSpace;
	float3 PositionScreenSpace;
	float3 PositionView;
	float3 PositionWorld;
	float4 Diffuse;
	float3 Normal;
	float3 SpecularColor;
	float Roughness;
	float Metallic;
	float3 Emissive;
};

half RoughnessToMipLevel(float Roughness, int MipCount)
{
	// Level starting from 1x1 mip
	return MipCount - 6 - 1.15 * log2(Roughness);
	//solve for x z = y - 4 - 1.15 * log2(x)
	//log2(roughness) = (mipcount - 4 - mip)
}

float MipLevelToRoughness(int MipLevel, int MipCount)
{
	return pow(2.0f, (MipCount - 6.0f - MipLevel) / 1.15f);
}

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

float Diffuse_OrenNayar(float r2, float3 N, float3 L, float3 V, float NoL, float NoV)
{
	float A = 1.0 - 0.5 * (r2 / (r2 + 0.57));
	float B = 0.45 * (r2 / (r2 + 0.09));
	
	float angleNoL = acos(NoL);
	float angleNoV = acos(NoV);
	

	float alpha = max(angleNoL, angleNoV);
	float beta = min(angleNoL, angleNoV);
	float gamma = dot(V - N * dot(V, N), L - N * dot(L, N));

	return (A + B * max(0.0, gamma) * sin(alpha) * tan(beta)) * saturate(NoL);
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

float4 ImportanceSampleGGX(float2 E, float Roughness)
{
	float m = Roughness * Roughness;
	float m2 = m * m;
	float Phi = 2 * PI * E.x;
	float CosTheta = sqrt((1 - E.y) / (1 + (m2 - 1) * E.y));
	float SinTheta = sqrt(1 - CosTheta * CosTheta);
	float3 H;
	H.x = SinTheta * cos(Phi);
	H.y = SinTheta * sin(Phi);
	H.z = CosTheta;
	float d = (CosTheta * m2 - CosTheta) * CosTheta + 1;
	float D = m2 / (PI*d*d);
	float PDF = D * CosTheta;
	return float4(H, PDF);
}

float3 PrefilterEnvMap(TextureCube EnvMap, SamplerState EnvSampler, float Roughness, float3 R)
{
	float3 N = R;
	float3 V = R;
	float3 PrefilteredColor = 0;
	float TotalWeight = 0.0;
	const uint NumSamples = ENV_MAP_SAMPLES;

	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 H = ImportanceSampleGGX(Xi, Roughness, N);
		float3 L = 2 * dot(V, H) * H - V;
		float NoL = saturate(dot(N, L));

		if (NoL > 0)
		{
			PrefilteredColor += pow(abs(EnvMap.SampleLevel(EnvSampler, L, 0).rgb), 2.2f) * NoL;
			TotalWeight += NoL;
		}
	}

	return PrefilteredColor / TotalWeight;
}

float2 IntegrateBRDF(float Roughness, float NoV)
{
	float3 V;
	V.x = sqrt(1.0f - NoV * NoV); // sin
	V.y = 0;
	V.z = NoV; // cos
	float A = 0;
	float B = 0;

	const uint NumSamples = BRDF_SAMPLES;

	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 H = ImportanceSampleGGX(Xi.xy, Roughness).xyz;
		float3 L = 2 * dot(V, H) * H - V;
		float NoL = saturate(L.z);
		float NoH = saturate(H.z);
		float VoH = saturate(dot(V, H));
		if (NoL > 0)
		{
			float G = G_Smith(Roughness, NoV, NoL);
			float G_Vis = G * VoH / (NoH * NoV);
			float Fc = pow(1 - VoH, 5);
			A += (1 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}
	return float2(A, B) / NumSamples;
}

//float3 ApproximateSpecularIBL(TextureCube EnvMap, SamplerState EnvSampler, float3 SpecularColor, float Roughness, float3 N, float3 V)
//{
//	float NoV = abs(dot(N, V));
//	float3 R = 2 * dot(V, N) * N - V;
//	float3 PrefilteredColor = PrefilterEnvMap(EnvMap, EnvSampler, Roughness, R);
//	float2 EnvBRDF = IntegrateBRDF(Roughness, NoV);
//	return PrefilteredColor * (SpecularColor * EnvBRDF.x + EnvBRDF.y);
//}

float3 ApproximateSpecularIBL(TextureCube EnvMap, SamplerState EnvSampler, Texture2D<float2> LUT, int MipLevels, float3 SpecularColor, float Roughness, float3 N, float3 V)
{
	float NoV = abs(dot(N, V));
	float3 R = 2 * dot(V, N) * N - V;
	float3 PrefilteredColor = EnvMap.SampleLevel(EnvSampler, R, MipLevels - RoughnessToMipLevel(Roughness, MipLevels)).rgb;
	float2 EnvBRDF = saturate(LUT.SampleLevel(EnvSampler, saturate(float2(Roughness - 0.002, NoV - 0.002)), 0));
	return PrefilteredColor * (SpecularColor * EnvBRDF.x + EnvBRDF.y);
}

float3 SpecularIBL(TextureCube EnvMap, SamplerState EnvSampler, float3 SpecularColor, float Roughness, float3 N, float3 V)
{
	float3 SpecularLighting = 0;
	const uint NumSamples = ENV_MAP_SAMPLES;

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
			float3 SampleColor = abs(EnvMap.SampleLevel(EnvSampler, L, 0).rgb);
			float G = G_Smith(Roughness, NoV, NoL);
			float Fc = pow(1 - VoH, 5);
			float3 F = (1 - Fc) * SpecularColor + Fc;

			SpecularLighting += SampleColor * F * G * VoH / (NoH * NoV);
		}
	}

	return SpecularLighting / NumSamples;
}

void AccumulateBRDF(SURFACE_DATA surface, PointLight light, float3 toEye, inout float3 finalColor)
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

	//float diffuseContrib = saturate(NoL);
	float diffuseContrib = saturate(Diffuse_OrenNayar(roughness2, surface.Normal, toLight, toEye, NoL, NoV));

	float D = D_GGX(roughness2, NoH);
	float G = G_Smith(surface.Roughness, NoV, NoL);
	float3 F = F_Schlick(lerp(float3(0.0f, 0.0f, 0.0f), surface.SpecularColor, surface.Metallic), VoH);

	float3 specContrib = (D*G*F) * NoL;//TODO: This is not correct, but fixes the specular falloff to some degree for the time being

	finalColor += (light.Color / PI) * surface.Diffuse.rgb * light.Intensity * diffuseContrib * oneOverDistSq * (1.0f - surface.Metallic);//Diffuse
	finalColor += light.Color * light.Intensity * saturate(specContrib) * oneOverDistSq * lerp(float3(1.0f, 1.0f, 1.0f), surface.SpecularColor, surface.Metallic);//Specular
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

#endif