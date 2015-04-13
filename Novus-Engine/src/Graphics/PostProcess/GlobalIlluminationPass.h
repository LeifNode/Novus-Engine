//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_PREFILTERED_ENVIRONMENT_MAP_H
#define NOVUS_PREFILTERED_ENVIRONMENT_MAP_H

#include "Application/Common.h"
#include "Math/Math.h"

namespace novus
{

class D3DRenderer;
class Shader;
class Texture2D;
class GBuffer;
class ShadowMapRenderTarget;
class VoxelRadianceVolume;

class GlobalIlluminationPass
{
	struct CBVoxelGI
	{
		Matrix4 WorldToVoxelVolume;

		Vector3i VoxelVolumeDim;
		int VoxelMipCount;

		Vector3 pad;
		float VoxelScale; //Size of one voxel in meters
	};


	struct CBShadowPass
	{
		Vector3 LightDirection;
		float ShadowIntensity;

		Vector4 LightColor;

		Matrix4 WorldToShadow;
	};

public:
	GlobalIlluminationPass();
	~GlobalIlluminationPass();

	void Init(int width, int height);

	void OnResize(int width, int height);

	void setGBuffer(GBuffer* gbuffer);
	void setShadowMap(ShadowMapRenderTarget* shadowMap);
	void setVoxelVolume(VoxelRadianceVolume* voxelVolume);
	void setLightDirection(const Vector3& direction);
	Vector3 getLightDirection() const { return mLightDirection; }

	Texture2D* getRenderTarget() const { return mpDestinationTexture; }

	void Execute(D3DRenderer* renderer);

private:
	void InitShader(D3DRenderer* renderer);
	void InitConstantBuffers(D3DRenderer* renderer);
	void InitDestinationTexture(D3DRenderer* renderer, int width, int height);
	void InitSamplerState(D3DRenderer* renderer);

private:
	Vector3 mLightDirection;

	Shader* mpGIShader;

	GBuffer* mpSourceGBuffer;
	ShadowMapRenderTarget* mpSourceShadowMap;
	VoxelRadianceVolume* mpRadianceVolume;

	Texture2D* mpDestinationTexture;

	ID3D11Buffer* mpConstantBuffer;
	ID3D11Buffer* mpVXGIBuffer;
	ID3D11SamplerState* mpConeTracingSamplerState;

};

}

#endif