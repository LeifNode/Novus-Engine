//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_FONT_H
#define NOVUS_FONT_H

#include "Utils/RectangleBinPacker.h"
#include "Graphics/Textures/Texture2D.h"
#include "Utils/Primitives/Rectangle.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace novus
{

namespace FontType
{
enum Type
{
	Normal,
	Bold,
	Italic,
	BoldItalic,
	COUNT,
};
}

class Font
{
	friend class FontManager;
public:
	struct Glyph
	{
		int character;
		int pointSize;
		int left;
		int top;
		int width;
		int height;
		int advance;
		int bearing;
	};

public:
	std::string getFontPath(FontType::Type type) const { return mFontPaths[type]; }

	Rectangle MeasureString(const std::string& str, int pointSize, FontType::Type type = FontType::Normal);
	Rectangle MeasureString(const std::string& str, int pointSize, FontType::Type type, int verticalOffset, const Rectangle& bounds);

	const Glyph* getCharacter(int charCode, int pointSize = 12, FontType::Type type = FontType::Normal);

	int getTextureWidth() const { return mpFontTexture->getWidth(); }
	int getTextureHeight() const { return mpFontTexture->getHeight(); }

	bool LoadCharacter(int charCode, int pointSize, FontType::Type type = FontType::Normal);
	bool LoadGlyphs(int pointSize, FontType::Type type = FontType::Normal);

	float getUsedSpace() { return mBinPacker.getFillPercent(); }
	Texture2D* getFontTexture() const { return mpFontTexture; }

private:
	Font();
	~Font();

	Font(const Font&);
	Font& operator= (const Font&);

	void Init(int width, int height);
	void LoadFont(const std::string& path, FT_Library& library, FontType::Type type = FontType::Normal);

private:
	//int for char code and int for point size
	std::map<std::pair<int, int>, Glyph*> mCharacterMaps[FontType::COUNT];

	RectangleBinPacker mBinPacker;
	FT_Face mFaces[FontType::COUNT];

	int mNumMipmaps;
	int mHorizontalOffset; //Used to avoid characters with short widths from overlapping
	std::string mFontPaths[FontType::COUNT];

	Texture2D* mpFontTexture;
};

}

#endif