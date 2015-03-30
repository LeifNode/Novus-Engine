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

class ShadowMapRenderTarget : public IRenderTarget
{
public:
	ShadowMapRenderTarget();
	~ShadowMapRenderTarget();

	void Init(int width, int height);

	void BindTarget(D3DRenderer* renderer) override;
	void UnbindTarget(D3DRenderer* renderer) override;

	RenderPass::Type GetRenderPass() const override;

	bool ShouldRender(const Actor* actor) const override;



private:
	mutable bool mTransformDirty;

	Matrix4 mProjectionMatrix;
	mutable Matrix4 mViewMatrix;
	mutable Matrix4 mTransform;
};

}


#endif