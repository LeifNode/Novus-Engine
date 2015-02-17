#include "TextRenderer.h"
#include "Graphics/D3DRenderer.h"
#include "Application/Common.h"
#include "Application/EngineStatics.h"
#include "Resources/Font/FontManager.h"
#include "Resources/Font/Font.h"
#include "Application/NovusApplication.h"

namespace novus
{

TextRenderer::TextRenderer(int capacity)
	:mTextChanged(false),
	mpVertexBuffer(NULL),
	mMaxCharCount(capacity),
	mpFont(NULL),
	mTextSize(12),
	mpTextShader(NULL),
	mpSampler(NULL),
	mVerticalSpacing(0.018f)
{
}

TextRenderer::~TextRenderer()
{
	ReleaseCOM(mpVertexBuffer);
	ReleaseCOM(mpSampler);
}

void TextRenderer::Init()
{
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA initData;

	ZeroMemory(&desc, sizeof(desc));

	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(TextQuadVert)* mMaxCharCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	TextQuadVert* pVertArray = NE_NEW TextQuadVert[mMaxCharCount]();
	memset(pVertArray, 0, sizeof(TextQuadVert)* mMaxCharCount);//Clear array

	pVertArray[0].Dimensions = Vector2(2.0f, 2.0f);
	pVertArray[0].TexTL = Vector2(0.0f, 0.0f);
	pVertArray[0].TexBR = Vector2(1.0f, 1.0f);

	initData.pSysMem = pVertArray;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	HRESULT hr = EngineStatics::getRenderer()->device()->CreateBuffer(&desc, &initData, &mpVertexBuffer);
	if (FAILED(hr))
		std::cout << "Failed to create text renderer vertex buffer.\n";

	NE_DELETEARR(pVertArray);

	InitShader();
	InitSampler();
}

void TextRenderer::InitShader()
{
	mpTextShader = EngineStatics::getRenderer()->getShader("../Shaders/Text.hlsl");

	if (mpTextShader == NULL)
	{
		ShaderInfo shaderInfo[] = {
			{ ShaderType::Vertex, "VS" },
			{ ShaderType::Geometry, "GS" },
			{ ShaderType::Pixel, "PS" },
			{ ShaderType::None, NULL }
		};

		D3D11_INPUT_ELEMENT_DESC vertexDescription[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "DIMENSION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		mpTextShader = EngineStatics::getRenderer()->LoadShader(L"../Shaders/Text.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_POINTLIST, vertexDescription, ARRAYSIZE(vertexDescription));
	}
}

void TextRenderer::InitSampler()
{
	D3DRenderer* renderer = EngineStatics::getRenderer();
	ID3D11DeviceContext* context = renderer->context();

	D3D11_SAMPLER_DESC samplerDesc;

	ZeroMemory(&samplerDesc, sizeof(samplerDesc));

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT result = renderer->device()->CreateSamplerState(&samplerDesc, &mpSampler);
	if (FAILED(result))
		NE_CRITICAL("Failed to create sampler state for font.", "TextRenderer");
}

void TextRenderer::UpdateVertexBuffer()
{
	if (mTextChanged)
	{
		FontManager* fontManager = EngineStatics::getFontManager();
		D3DRenderer* renderer = EngineStatics::getRenderer();

		TextQuadVert* pVertArray = NE_NEW TextQuadVert[mMaxCharCount]();
		memset(pVertArray, 0, sizeof(TextQuadVert)* mMaxCharCount);//Clear array
		float textOffset = 0.0f;
		float heightOffset = 0.0f;

		int skippedCount = 0;
		int lineCount = 0;

		for (int i = 0; i < mText.size(); i++)
		{
			char ch = mText[i];
			lineCount++;
			if (ch == '\n')
			{
				textOffset = 0;
				heightOffset -= mVerticalSpacing;
				skippedCount++;
				continue;
			}

			if (lineCount > 450)
			{
				textOffset = 0;
				heightOffset -= mVerticalSpacing;
				lineCount = 0;
			}

			const Font::Glyph* glyph = mpFont->getCharacter(ch, mTextSize);

			if (glyph == NULL) //Character not loaded
			{
				std::cout << "Char null" << std::endl;
				skippedCount++;
				continue;
			}

			int index = i - skippedCount;
			float width = (float)mpFont->getTextureWidth();
			float height = (float)mpFont->getTextureHeight();

			pVertArray[index].TexTL = Vector2(glyph->left / width, glyph->top / height);
			pVertArray[index].TexBR = Vector2((glyph->left + glyph->width) / width, (glyph->top + glyph->height) / height);
			pVertArray[index].Dimensions = Vector2(glyph->width / 12000.0f, glyph->height / 12000.0f);//Transform pixel counts into meters assuming 300dpi
			pVertArray[index].Position = Vector3(textOffset, heightOffset - (glyph->height - glyph->bearing) / 12000.0f, 0.0f);
			//pVertArray[index].Dimensions = Vector2(1.0f, 1.0f);
			//pVertArray[index].Position = Vector3(0.0f, 0.0f, 0.0f);

			textOffset += (glyph->advance + 2) / 12000.0f;
		}

		D3D11_MAPPED_SUBRESOURCE resource;
		ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		renderer->context()->Map(mpVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

		memcpy(resource.pData, pVertArray, sizeof(TextQuadVert)* mMaxCharCount);

		renderer->context()->Unmap(mpVertexBuffer, 0);

		NE_DELETEARR(pVertArray);

		mTextChanged = false;
	}
}

void TextRenderer::Render(D3DRenderer* renderer)
{
	UpdateVertexBuffer();

	renderer->setShader(mpTextShader);
	renderer->setSampler(0, mpSampler);
	renderer->setTextureResource(0, mpFont->getFontTexture());
	renderer->BindPerFrameBuffer();

	unsigned int stride = sizeof(TextQuadVert), offset = 0;

	renderer->setBlendState(true);
	renderer->context()->IASetVertexBuffers(0, 1, &mpVertexBuffer, &stride, &offset);
	renderer->context()->Draw((UINT)mText.size(), 0);
}

void TextRenderer::setText(const std::string& text)
{
	if (text.size() < mMaxCharCount && mText != text)
	{
		mTextChanged = true;
		mText = text;
	}
}

}