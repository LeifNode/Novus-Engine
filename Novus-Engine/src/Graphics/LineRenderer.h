//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_LINE_RENDERER_H
#define NOVUS_LINE_RENDERER_H

#include "Application/Common.h"
#include "Math/Math.h"
#include "Utils/Containers/PointList.h"

namespace novus
{

class Shader;
class D3DRenderer;

namespace LineDrawMode
{
	enum Type
	{
		LineList,
		LineStrip,
		NativeLineList,
		NativeLineStrip,
		COUNT,
	};
}


class LineRenderer
{
	struct CBPerLine
	{
		unsigned int PointCount;
		Vector3u pad;
		Vector4 Color;
		float Width;
		Vector3 pad2;
	};

public:
	LineRenderer();
	~LineRenderer();

	void ReloadPoints();

	void setDrawMode(LineDrawMode::Type mode);
	void setLineWidth(float width);
	void setLineColor(const Vector4& color);

	void Init();

	void Render(D3DRenderer* renderer);

	PointList points;

private:
	void generateIndices(UINT** ppArrayOut, UINT* pIndexCount);

private:
	Shader* mpLineShader;
	ID3D11Buffer* mpPerLineBuffer;

	float mLineWidth;
	Vector4 mLineColor;

	ID3D11Buffer* mpVertexBuffer;
	ID3D11Buffer* mpIndexBuffer;

	LineDrawMode::Type mMode;
};

}

#endif