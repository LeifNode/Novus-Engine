#include "TextRenderer.h"
#include "Graphics/D3DRenderer.h"
#include "Application/Common.h"
#include "Application/EngineStatics.h"
#include "Resources/Font/FontManager.h"
#include "Resources/Font/Font.h"

namespace novus
{

TextRenderer::TextRenderer(int capacity)
	:mTextChanged(false),
	mpVertexBuffer(NULL),
	mMaxCharCount(capacity),
	mpFont(NULL),
	mTextSize(12)
{
}

TextRenderer::~TextRenderer()
{
	ReleaseCOM(mpVertexBuffer);
}

void TextRenderer::Initialize()
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
}

void TextRenderer::updateVertexBuffer()
{
	if (mTextChanged)
	{
		FontManager* fontManager = EngineStatics::getFontManager();
		D3DRenderer* renderer = EngineStatics::getRenderer();

		TextQuadVert* pVertArray = NE_NEW TextQuadVert[mMaxCharCount]();
		memset(pVertArray, 0, sizeof(TextQuadVert)* mMaxCharCount);//Clear array
		float textOffset = 0.0f;
		float heightOffset = 0.4f;

		int skippedCount = 0;
		int lineCount = 0;

		for (int i = 0; i < mText.size(); i++)
		{
			char ch = mText[i];
			lineCount++;
			if (ch == '\n')
			{
				textOffset = 0;
				heightOffset -= 0.018f;
				skippedCount++;
				continue;
			}

			if (lineCount > 450)
			{
				textOffset = 0;
				heightOffset -= 0.018f;
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
	updateVertexBuffer();

	renderer->setTextureResource(0, mpFont->getFontTexture());

	CBPerObject perObject;

	//perObject.World =  XMMatrixScaling(-1.0f, 1.0f, 1.0f) * mTransform.getTransform();
	Vector3 worldOffset = renderer->getPerFrameBuffer()->HeadPosition;
	perObject.World = Matrix4::Scale(-1.0f, 1.0f, 1.0f) * mTransform.GetTransform() * Matrix4::Translate(0.25f, -0.2f, 0.3f) * Matrix4::Translate(worldOffset);
	perObject.WorldInvTranspose = Matrix4::Inverse(Matrix4::Transpose(perObject.World));
	perObject.WorldViewProj = perObject.World * renderer->getPerFrameBuffer()->ViewProj;
	//perObject.WorldViewProj = perObject.World * XMMatrixOrthographicLH(1.0f, 0.5625f, 0.0f, 1.0f);

	renderer->setPerObjectBuffer(perObject);

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