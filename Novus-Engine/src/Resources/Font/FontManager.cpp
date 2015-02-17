#include "FontManager.h"
#include "Graphics/D3DRenderer.h"
#include "Application/Common.h"

namespace novus
{

FontManager::FontManager()
{}

FontManager::~FontManager()
{
	FT_Error error = FT_Done_FreeType(mFTLibrary);

	if (error)
		std::cout << "Failed to unload freetype.\n";

	for (auto it = mFontMap.begin(); it != mFontMap.end(); ++it)
	{
		NE_DELETE(it->second);
	}
}

bool FontManager::Init()
{
	memset(&mFTLibrary, 0, sizeof(mFTLibrary));
	FT_Error error = FT_Init_FreeType(&mFTLibrary);

	if (error)
	{
		NE_CRITICAL("Failed to initialize freetype.", "FontManager");
		return false;
	}

	return true;
}

Font* FontManager::LoadFont(const std::string& fontKey,
	const std::string& fontPath,
	const std::string& fontPathBold,
	const std::string& fontPathItalic,
	const std::string& fontPathBoldItalic)
{
	auto it = mFontMap.find(fontKey);
	if (it != mFontMap.end())
	{
		std::cout << "Font already loaded.\n";
		return it->second;
	}

	Font* newFont = NE_NEW Font();
	//newFont->Init(4096, 4096);
	newFont->Init(2048, 2048);

	newFont->LoadFont(fontPath, mFTLibrary);

	if (fontPathBold != "")
		newFont->LoadFont(fontPathBold, mFTLibrary, FontType::Bold);
	if (fontPathItalic != "")
		newFont->LoadFont(fontPathItalic, mFTLibrary, FontType::Italic);
	if (fontPathBoldItalic != "")
		newFont->LoadFont(fontPathBoldItalic, mFTLibrary, FontType::BoldItalic);

	mFontMap.insert(std::make_pair(fontKey, newFont));

	return newFont;
}

Font* FontManager::GetFont(const std::string& key)
{
	auto it = mFontMap.find(key);

	if (it != mFontMap.end())
		return it->second;

	return NULL;
}

}//namespace novus