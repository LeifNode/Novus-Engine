//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_FONT_MANAGER_H
#define NOVUS_FONT_MANAGER_H

#include "Font.h"

namespace novus
{
class FontManager
{
public:
	FontManager();
	~FontManager();

	bool Init();

	Font* LoadFont(const std::string& fontKey,
		const std::string& fontPath,
		const std::string& fontPathBold = "",
		const std::string& fontPathItalic = "",
		const std::string& fontPathBoldItalic = "");

	Font* GetFont(const std::string& key);

private:
	//void initializeSampler();
	//void initializeShader();

private:
	FT_Library mFTLibrary;

	std::map<std::string, Font*> mFontMap;
};
}

#endif