//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_SKYBOX_RENDERER_H
#define NOVUS_SKYBOX_RENDERER_H

#include "Application/Common.h"
#include "Graphics/MeshRenderer.h"
#include "Math/Vector3_t.h"

namespace novus
{

class SkyboxRenderer
{
public:
	SkyboxRenderer();
	~SkyboxRenderer();

	void Init(const wchar_t* cubemapPath);

	void Render(class D3DRenderer* renderer);

private:
	class Shader* mpSkyboxShader;
	class Texture2D* mpCubemap;
	ID3D11SamplerState* mpSamplerState;
	MeshRenderer<Vector3>* mpMeshRenderer;
};

}

#endif