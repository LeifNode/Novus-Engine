//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_VOXEL_RADIANCE_VOLUME_H
#define NOVUS_VOXEL_RADIANCE_VOLUME_H

#include "Application/Common.h"
#include "Math/Math.h"
#include "Graphics/VXGI/VoxelGICommon.h"
#include "Graphics/ConstantBuffer.h"

namespace novus
{

class VoxelVolumeRenderTarget;
class ShadowMapRenderTarget;
class Texture2D;
class Texture3D;
class Shader;
class D3DRenderer;

class VoxelRadianceVolume
{
	struct CBLightInjectionPass
	{
		Vector3 LightDirection;
		float pad;

		Vector3 LightColor;
		float LightIntensity;

		Matrix4 ShadowToWorld;
		Matrix4 ShadowToVoxelVolume;

		Vector2i ShadowMapDimensions;
		Vector2i pad2;

		Vector3i VoxelVolumeDimensions;
		int pad3;
	};

public:
	VoxelRadianceVolume();
	~VoxelRadianceVolume();

	void Init(VoxelVolumeRenderTarget* sourceVolume, ShadowMapRenderTarget* sourceShadowMap);

	Texture3D* getRadianceVolume() const { return mpVoxelRadianceVolume; }
	Texture3D* getRadianceMips() const { return mpAnisotropicMips; }

	VoxelVolumeRenderTarget* getSourceVolume() const { return mpSourceVolume; }

	void InjectRadiance(D3DRenderer* renderer);

private:
	void InitShaders(D3DRenderer* renderer);
	void InitRadianceVolume(D3DRenderer* renderer);
	void InitConstantBuffers(D3DRenderer* renderer);
	void InitMipUAVs(D3DRenderer* renderer);

	void CopyVoxelResource();

	void ClearColor(D3DRenderer* renderer);

	void FilterMips(D3DRenderer* renderer);
	void FilerMipLevel(D3DRenderer* renderer, int level);
	void FilterBaseMip(D3DRenderer* renderer);
	void FilerAnisotropicMipLevel(D3DRenderer* renderer, int level);

private:
	Shader* mpRadianceInjectionShader;
	Shader* mpVoxelFilterShader;
	Shader* mpVoxelAnisotropicFilterShader;
	Shader* mpVoxelFilterBaseMipShader;
	Shader* mpCopyOcclusionShader;

	ConstantBuffer<CBVoxelGI> mFilterBuffer;
	ConstantBuffer<CBLightInjectionPass> mRadianceInjectionBuffer;

	Texture2D* mpDebugPositionOut;
	Texture3D* mpVoxelRadianceVolume;
	Texture3D* mpAnisotropicMips;

	VoxelVolumeRenderTarget* mpSourceVolume;
	ShadowMapRenderTarget* mpSourceShadowMap;

	std::vector<ID3D11UnorderedAccessView*> mMipUavs;
	std::vector<ID3D11UnorderedAccessView*> mAnisotropicMipUavs;
};

}

#endif