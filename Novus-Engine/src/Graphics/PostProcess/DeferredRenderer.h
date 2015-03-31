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
#include "Graphics/LightManager.h"
#include "Graphics/Textures/Texture2D.h"

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

	LightManager* getLightManager() const { return mpLightManager; }

private:
	int mMaxLightCount;

	class Shader* mpTiledDeferredShader;
	class Shader* mpLUTShader;
	class Shader* mpDebugOutputShader;
	class Texture2D* mpHDRRenderTarget;
	class Texture2D* mpBRDFLUT;
	class PrefilteredEnvironmentMap* mpEnvMap;
	ID3D11SamplerState* mpPointSampler;
	ID3D11SamplerState* mpEnvironmentSampler;

	StructuredBuffer<PointLight> mLightBuffer;

	LightManager* mpLightManager;
};

}

#endif