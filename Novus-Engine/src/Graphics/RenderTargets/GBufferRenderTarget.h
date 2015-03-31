//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_GBUFFER_RENDER_TARGET_H
#define NOVUS_GBUFFER_RENDER_TARGET_H

#include "Application/Common.h"
#include "Math/Math.h"
#include "IRenderTarget.h"

namespace novus
{

class Texture2D;
class D3DRenderer;
class Actor;
class GBuffer;

class GBufferRenderTarget : public IRenderTarget
{
public:
	//True if this buffer is prioritized as a viewport that will render directly to the screen
	GBufferRenderTarget(bool primaryRenderTarget);
	~GBufferRenderTarget();

	void Init(int width, int height);

	void BindTarget(D3DRenderer* renderer) override;
	void UnbindTarget(D3DRenderer* renderer) override;

	RenderPass::Type GetRenderPass() const override;

	bool ShouldRenderActor(const Actor* actor) const override;

	Matrix4 getViewTransform() const;

	bool IsPrimaryRenderTarget() const;
	void setIsPrimaryRenderTarget(bool isPrimaryRenderTarget);

	GBuffer* getGBuffer() const { return mpGBuffer; }

protected:
	GBuffer* mpGBuffer;

	bool mIsPrimaryRenderTarget;

	//TODO: Generalize the Camera class to handle these transforms for Orthographic & Perspective views
	Matrix4 mView;
	Matrix4 mProjection;
};

}


#endif