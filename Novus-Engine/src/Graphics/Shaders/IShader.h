//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_ISHADER_H
#define NOVUS_ISHADER_H

#include <string>

namespace novus
{

class D3DRenderer;

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

struct ShaderInfo
{
	ShaderType::Type type;
	const char* entrypoint;
};

class IShader
{
public:
	virtual ShaderType::Type getShaderType() const = 0;

	virtual void Bind(D3DRenderer* renderer) = 0;
};

}

#endif