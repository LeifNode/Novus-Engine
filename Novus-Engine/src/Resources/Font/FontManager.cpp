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
		std::cout << "Failed to initialize freetype.\n";
		return false;
	}

	return true;
}

//void FontManager::initializeSampler()
//{
//	D3DRenderer* renderer = gpApplication->getRenderer();
//	ID3D11DeviceContext* context = renderer->context();
//
//	D3D11_SAMPLER_DESC samplerDesc;
//
//	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
//
//	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
//	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
//	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
//	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
//	samplerDesc.MipLODBias = 0;
//	samplerDesc.MaxAnisotropy = 1;
//	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
//	samplerDesc.MinLOD = 0;
//	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
//
//	HRESULT result = renderer->device()->CreateSamplerState(&samplerDesc, &mpSampler);
//	if (FAILED(result))
//		std::cout << "Failed to create sampler state for font.\n";
//}
//
//void FontManager::initializeShader()
//{
//	Shader::ShaderInfo shaderInfo[] = {
//		{ Shader::SHADER_TYPE_VERTEX, "VS" },
//		{ Shader::SHADER_TYPE_GEOMETRY, "GS" },
//		{ Shader::SHADER_TYPE_PIXEL, "PS" },
//		{ Shader::SHADER_TYPE_NONE, NULL }
//	};
//
//	D3D11_INPUT_ELEMENT_DESC vertexDescription[] =
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "DIMENSION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//
//	mpTextRenderShader = gpApplication->getRenderer()->loadShader(L"Shaders/text.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_POINTLIST, vertexDescription, ARRAYSIZE(vertexDescription));
//}

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