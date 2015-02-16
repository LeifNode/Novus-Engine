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
#include "Math/Vector2_t.h"

namespace novus
{

class D3DRenderer;
class Shader;
class Texture2D;

class PrefilteredEnvironmentMap
{
	struct CBFilterEnvMap
	{
		Vector2i SourceDimensions;
		float Roughness;
		float pad;

		Vector3 Up;
		float pad2;
		Vector3 Forward;
		float pad3;
	};

public:
	PrefilteredEnvironmentMap();
	~PrefilteredEnvironmentMap();

	void Init(const std::wstring& sourcePath);

	void Bind(D3DRenderer* renderer, int index);

private:
	void InitConstantBuffer(D3DRenderer* renderer);
	void InitSourceTexture(D3DRenderer* renderer, const std::wstring& path);
	void InitDestinationTexture(D3DRenderer* renderer);
	void InitFilterShader(D3DRenderer* renderer);
	void FilterSourceTexture(D3DRenderer* renderer);

private:
	Shader* mpFilterShader;
	ID3D11Buffer* mpFilterBuffer;

	Texture2D* mpSourceTexture;
	ID3D11Texture2D* mpFilteredTexture;

	ID3D11ShaderResourceView* mpFilteredTextureSRV;
	ID3D11UnorderedAccessView** mpUAVArray;

	static const int maxMipFilterLevel = 5;
};

}

#endif