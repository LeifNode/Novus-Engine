//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_DEFERRED_RENDERER_H
#define NOVUS_DEFERRED_RENDERER_H

#include "Application/Common.h"
#include "Graphics/StructuredBuffer.h"
#include "Graphics/Lights.h"

namespace novus
{

class DeferredRenderer
{
public:
	DeferredRenderer();
	~DeferredRenderer();

	void Init(class D3DRenderer* renderer, int width, int height);

	void Update(float dt);

	void RenderDeferredShading(class D3DRenderer* renderer);

	void RenderDebugOutput(class D3DRenderer* renderer);

	class Texture2D* getHDRRenderTarget() const { return mpHDRRenderTarget; }

private:
	class Shader* mpTiledDeferredShader;
	class Shader* mpDebugOutputShader;
	class Texture2D* mpHDRRenderTarget;
	ID3D11SamplerState* mpPointSampler;

	StructuredBuffer<PointLight> mLightBuffer;

	std::vector<PointLight> mTestPointLights;
};

}

#endif