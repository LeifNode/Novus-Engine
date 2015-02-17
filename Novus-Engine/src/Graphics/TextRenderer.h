//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_TEXT_RENDERER_H
#define NOVUS_TEXT_RENDERER_H

#include "Application/Common.h"
#include "Math/Vector2_t.h"
#include "Math/Vector3_t.h"
#include "Math/Transform.h"

namespace novus
{
class D3DRenderer;
class Font;
class Shader;

struct TextQuadVert
{
	Vector3 Position;
	Vector2 TexTL;
	Vector2 TexBR;
	Vector2 Dimensions;
};

class TextRenderer
{
public:
	TextRenderer(int capacity = 500);
	~TextRenderer();

	void Init();

	void Render(D3DRenderer* renderer);

	void setTextWrap(bool enabled) { mTextWrapEnabled = enabled; }
	bool getTextWrap() const { return mTextWrapEnabled; }

	void setText(const std::string& text);
	std::string getText() const { return mText; }
	int getCharCount() const { return (int)mText.size(); }
	int getMaxCharCount() const { return mMaxCharCount; }

	void setFont(Font* font) { mpFont = font; mTextChanged = true; }
	Font* getFont() const { return mpFont; }

	int getTextSize() const { return mTextSize; }
	void setTextSize(int size) { mTextSize = size; }

	void setVerticalSpacing(float spacing) { mVerticalSpacing = spacing; mTextChanged = true; }

private:
	void InitShader();
	void InitSampler();
	void UpdateVertexBuffer();

public:
	Transform mTransform;

private:
	Shader* mpTextShader;

	Font* mpFont;

	bool mTextWrapEnabled;
	float mMaxWidth;
	float mVerticalSpacing;

	int mMaxCharCount;
	int mTextSize;

	bool mTextChanged;
	std::string mText;

	ID3D11Buffer* mpVertexBuffer;
	ID3D11SamplerState* mpSampler;
};
}

#endif