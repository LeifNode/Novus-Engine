//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_VOXEL_RADIANCE_VOLUME_H
#define NOVUS_VOXEL_RADIANCE_VOLUME_H

namespace novus
{

class VoxelVolumeRenderTarget;
class ShadowMapRenderTarget;
class Texture3D;
class Shader;

class VoxelRadianceVolume
{
public:
	VoxelRadianceVolume();
	~VoxelRadianceVolume();

	void Init(VoxelVolumeRenderTarget* sourceVolume, ShadowMapRenderTarget* sourceShadowMap);

private:
	void CopyVoxelResource();

	void ClearColor();
	void InjectRadiance();

	void FilterMips();
	void FilerMipLevel(int level);

private:
	Shader* mpRadianceInjectionShader;
	Shader* mpVoxelFilterShader;

	Texture3D* mpVoxelRadianceVolume;

	VoxelVolumeRenderTarget* mpSourceVolume;
	ShadowMapRenderTarget* mpSourceShadowMap;
};

}

#endif