#include "Font.h"
#include "Math/Math.h"
#include "Graphics/D3DRenderer.h"
#include "Application/EngineStatics.h"

namespace novus
{

Font::Font()
	:mpFontTexture(NULL)
{
	mHorizontalOffset = 2;
	mNumMipmaps = 0;

	for (int i = 0; i < FontType::COUNT; i++)
		mFaces[i] = NULL;
}

Font::~Font()
{
	NE_DELETE(mpFontTexture);

	for (int i = 0; i < FontType::COUNT; i++)
	{
		if (mFaces[i] != NULL)
		{
			//FT_Done_Face(mFaces[i]);
			mFaces[i] = NULL;
		}

		auto end = mCharacterMaps[i].end();
		for (auto it = mCharacterMaps[i].begin(); it != end; ++it)
		{
			NE_DELETE(it->second);
		}

		mCharacterMaps[i].clear();
	}
}

void Font::Init(int width, int height)
{
	NE_DELETE(mpFontTexture);

	mBinPacker.Init(width, height);

	D3DRenderer* renderer = EngineStatics::getRenderer();

	D3D11_TEXTURE2D_DESC textureDesc;

	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = Math::MipMapCount(width, height);
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	mNumMipmaps = textureDesc.MipLevels;

	mpFontTexture = renderer->CreateTexture(&textureDesc);

	mpFontTexture->setDebugName("[Font]");

	if (mpFontTexture == NULL)
		std::cout << "Failed to create font texture.\n";
}

void Font::LoadFont(const std::string& path, FT_Library& library, FontType::Type type)
{
	if (mFaces[type])
		FT_Done_Face(mFaces[type]);

	FT_Error error = FT_New_Face(library, path.c_str(), 0, &mFaces[type]);

	if (error == FT_Err_Unknown_File_Format)
	{
		NE_ERROR("Invalid font format.", "Font");
	}
	else if (error)
	{
		NE_ERROR("Error loading font.", "Font");
	}

	mFontPaths[type] = path;
}

bool Font::LoadCharacter(int charCode, int pointSize, FontType::Type type)
{
	if (mFaces[type] == NULL)
	{
		std::cout << "Font type not loaded defaulting to normal font\n";
		type = FontType::Normal;
	}

	if (mCharacterMaps[type].find(std::make_pair(charCode, pointSize)) != mCharacterMaps[type].end())
	{
		std::cout << "Character '" << (char)charCode << "' is already loaded\n";
		return false;
	}

	FT_Error error = FT_Set_Char_Size(mFaces[type], 0, pointSize * 64, 300, 300);
	assert(error == 0);

	int glyphIndex = FT_Get_Char_Index(mFaces[type], charCode);
	assert(glyphIndex != 0);

	error = FT_Load_Glyph(mFaces[type], glyphIndex, FT_LOAD_DEFAULT);
	assert(error == 0);

	FT_Render_Glyph(mFaces[type]->glyph, FT_RENDER_MODE_NORMAL);
	/*assert(mFace->glyph->format == FT_GLYPH_FORMAT_BITMAP);*/

	FT_Bitmap* bitmap = &mFaces[type]->glyph->bitmap;
	assert(bitmap);

	int width = bitmap->width;
	int height = bitmap->rows;

	const int padding = 4;

	auto node = mBinPacker.Insert(width + padding, height + padding);

	if (node != NULL)
	{
		//Render to texture
		D3DRenderer* renderer = EngineStatics::getRenderer();

		D3D11_BOX box;
		box.left = node->x + padding / 2;
		box.right = box.left + width;
		box.top = node->y + padding / 2;
		box.bottom = box.top + height;
		box.front = 0;
		box.back = 1;

		if (width > 0 && height > 0)
			renderer->context()->UpdateSubresource(mpFontTexture->getTexture(), D3D11CalcSubresource(0, 0, mNumMipmaps), &box, bitmap->buffer, bitmap->pitch, 0);

		//Cache to map
		Glyph* newGlyph = NE_NEW Glyph;

		newGlyph->character = charCode;
		newGlyph->pointSize = pointSize;
		newGlyph->left = box.left;
		newGlyph->top = box.top;
		newGlyph->width = width;
		newGlyph->height = height;
		newGlyph->advance = mFaces[type]->glyph->advance.x / 64;
		newGlyph->bearing = mFaces[type]->glyph->metrics.horiBearingY / 64;


		mCharacterMaps[type].insert(std::make_pair(std::make_pair(charCode, pointSize), newGlyph));
	}
	else
	{
		std::cout << "Failed to pack character " << (char)charCode << " (" << (int)charCode << ")" << ".\n";
		return false;
	}

	return true;
}

bool Font::LoadGlyphs(int pointSize, FontType::Type type)
{
	for (int i = 32; i < 127; i++)
	{
		if (!LoadCharacter(i, pointSize, type))
			return false;
	}

	EngineStatics::getRenderer()->context()->GenerateMips(mpFontTexture->getResourceView());

	return true;
}

Rectangle Font::MeasureString(const std::string& str, int pointSize, FontType::Type type)
{
	return MeasureString(str, pointSize, type, 0, Rectangle());
}

Rectangle Font::MeasureString(const std::string& str, int pointSize, FontType::Type type, int verticalOffset, const Rectangle& bounds)
{
	Rectangle totalSize;

	unsigned int size = (unsigned int)str.size();
	int maxCharHeight = 0;

	for (unsigned int i = 0; i < size; i++)
	{
		const Glyph* character = getCharacter(str[i], pointSize, type);

		if (character == NULL)
			continue;

		int horizontalOffset = character->advance + mHorizontalOffset;

		if (bounds.width == 0 || horizontalOffset + totalSize.width < bounds.width)
		{
			totalSize.width += horizontalOffset;

			maxCharHeight = Math::Max(maxCharHeight, character->height - character->bearing);
		}
		else
		{
			maxCharHeight = 0;
			totalSize.height += verticalOffset;
		}
	}

	totalSize.height += maxCharHeight;//For any characters on the bottom line

	return totalSize;
}

const Font::Glyph* Font::getCharacter(int charCode, int pointSize, FontType::Type type)
{
	auto it = mCharacterMaps[type].find(std::make_pair(charCode, pointSize));

	if (it != mCharacterMaps[type].end())
		return it->second;

	return NULL;
}

}//namespace novus