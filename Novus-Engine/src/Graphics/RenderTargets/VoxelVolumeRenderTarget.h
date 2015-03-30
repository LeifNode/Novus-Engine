//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_VOXEL_VOLUME_RENDER_TARGET_H
#define NOVUS_VOXEL_VOLUME_RENDER_TARGET_H

#include "Application/Common.h"
#include "Math/Math.h"
#include "IRenderTarget.h"

namespace novus
{

class Texture3D;

class VoxelVolumeRenderTarget : public IRenderTarget
{
public:
	VoxelVolumeRenderTarget();
	~VoxelVolumeRenderTarget();

	void Init(int resolution);

	void setBounds(float width, float height, float depth);

	void setPosition(const Vector3& position);
	Vector3 getPosition() const { return mPosition; }

	int getResolution() const { return mResolution; }

	void BindTarget(D3DRenderer* renderer) override;

	void UnbindTarget(D3DRenderer* renderer) override;

	RenderPass::Type GetRenderPass() const override;

	bool ShouldRender(const Actor* actor) const override;

private:
	void BuildWorldToVolumeProj();

private:
	Texture3D* mpVoxelTexture;

	int mResolution;

	Vector3 mBounds;

	Vector3 mPosition;

	Matrix4 mProjection;
	Matrix4 mWorldToVolume;

	Matrix4 mTransform;
	Matrix4 mSampleTransform;

};

}

#endif