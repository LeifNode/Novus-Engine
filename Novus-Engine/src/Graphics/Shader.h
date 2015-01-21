//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef SHADER_H
#define SHADER_H

#include "Application/Common.h"

namespace novus
{
namespace ShaderType
{
	enum Type
	{
		None = -1,
		Pixel,
		Vertex,
		Geometry,
		Hull,
		Domain,
		Compute
	};
}

class Shader
{
	friend class D3DRenderer;

public:
	struct ShaderInfo
	{
		ShaderType::Type type;
		const char* entrypoint;
	};

public:
	~Shader();

	std::string getName() const { return mName; }

	bool hasVertexShader() const { return mpVertexShader != nullptr; }
	bool hasPixelShader() const { return mpPixelShader != nullptr; }
	bool hasGeometryShader() const { return mpGeometryShader != nullptr; }
	bool hasComputeShader() const { return mpComputeShader != nullptr; }
	bool hasHullShader() const { return mpHullShader != nullptr; }
	bool hasDomainShader() const { return mpDomainShader != nullptr; }

private:
	Shader(const std::string& name);

	//void bind(D3DRenderer* renderer);

private:
	std::string mName;

	D3D11_PRIMITIVE_TOPOLOGY mPrimitiveTopology;

	ID3D11InputLayout* mpInputLayout;

	ID3D11VertexShader* mpVertexShader;
	ID3D11PixelShader* mpPixelShader;
	ID3D11GeometryShader* mpGeometryShader;
	ID3D11ComputeShader* mpComputeShader;
	ID3D11HullShader* mpHullShader;
	ID3D11DomainShader* mpDomainShader;
};

}

#endif