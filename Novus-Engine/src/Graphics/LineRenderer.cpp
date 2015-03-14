#include "LineRenderer.h"
#include "Utils/Primitives/LineSegment.h"
#include "Application/EngineStatics.h"
#include "D3DRenderer.h"

namespace novus
{

LineRenderer::LineRenderer()
	:mpVertexBuffer(NULL),
	mpIndexBuffer(NULL)
{
	mMode = LineDrawMode::LineList;
	mLineWidth = 0.01f;
	mLineColor = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
}

LineRenderer::~LineRenderer()
{
	ReleaseCOM(mpVertexBuffer);
	ReleaseCOM(mpIndexBuffer);
	ReleaseCOM(mpPerLineBuffer);
}

void LineRenderer::setDrawMode(LineDrawMode::Type mode)
{
	if (mMode != mode)
	{
		mMode = mode;
		ReloadPoints();
	}
}

void LineRenderer::setLineWidth(float width)
{
	mLineWidth = width;
}

void LineRenderer::setLineColor(const Vector4& color)
{
	mLineColor = color;
}

void LineRenderer::generateIndices(UINT** ppArrayOut, UINT* pIndexCount)
{
	//Line list = 2 indices per point, Line strip = 4 indices per point
	UINT indexCount = (UINT)points.list.size() * (mMode == LineDrawMode::LineList ? 2 : 4);
	*pIndexCount = indexCount;

	UINT* indexArray = NE_NEW UINT[indexCount];

	//Line List indexing
	if (mMode == LineDrawMode::LineList)
	{
		if (points.list.size() > 1)
		{
			for (unsigned int i = 0; i < points.list.size() - 1; i += 2)
			{
				if (i > 0)
					indexArray[i * 2] = i - 2; //Change to 2 for smoothing
				else
					indexArray[i * 2] = i;

				indexArray[i * 2 + 1] = i;
				indexArray[i * 2 + 2] = i + 1;

				if (i < points.list.size() - 2)
					indexArray[i * 2 + 3] = i + 3; //Change to 3 for smoothing
				else
					indexArray[i * 2 + 3] = i + 1;
			}
		}
	}
	else if (mMode == LineDrawMode::LineStrip)
	{
		//Line strip indexing
		if (points.list.size() > 1)
		{
			for (int i = 0; i < points.list.size() - 1; i++)
			{
				if (i > 0)
					indexArray[i * 4] = i - 1;
				else
					indexArray[i * 4] = i;

				indexArray[i * 4 + 1] = i;
				indexArray[i * 4 + 2] = i + 1;

				if (i < points.list.size() - 2)
					indexArray[i * 4 + 3] = i + 2;
				else
					indexArray[i * 4 + 3] = i + 1;
			}
		}
	}

	*ppArrayOut = indexArray;
}

void LineRenderer::ReloadPoints()
{
	if (points.list.size() > 0)
	{
		ReleaseCOM(mpVertexBuffer);
		ReleaseCOM(mpIndexBuffer);

		//TODO: Make dynamic buffer that resizes automatically
		UINT* indexArr;
		UINT indexCount;

		generateIndices(&indexArr, &indexCount);

		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(Vector3)* (UINT)points.list.size();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &points.list[0];

		HR(EngineStatics::getRenderer()->device()->CreateBuffer(&bd, &initData, &mpVertexBuffer));

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(UINT)* indexCount;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA iinitData;

		iinitData.pSysMem = &indexArr[0];

		HR(EngineStatics::getRenderer()->device()->CreateBuffer(&ibd, &iinitData, &mpIndexBuffer));

		NE_DELETEARR(indexArr);
	}
}

void LineRenderer::Init()
{
	D3DRenderer* renderer = EngineStatics::getRenderer();

	mpLineShader = renderer->getShader("../Shaders/Line.hlsl");

	if (mpLineShader == NULL)
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
		};

		mpLineShader = renderer->LoadShader(L"../Shaders/Line.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ, vertexDescription, ARRAYSIZE(vertexDescription));
	}

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBPerLine);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	renderer->device()->CreateBuffer(&bd, NULL, &mpPerLineBuffer);
}

void LineRenderer::Render(D3DRenderer* renderer)
{
	if (mpVertexBuffer != NULL)
	{
		renderer->setShader(mpLineShader);

		CBPerFrame perFrame = *renderer->getPerFrameBuffer();
		renderer->setPerFrameBuffer(perFrame);

		CBPerObject perObject;
		perObject.World = Matrix4(1.0f);
		perObject.WorldInvTranspose = Matrix4::Inverse(Matrix4::Transpose(perObject.World));
		perObject.WorldViewProj = perObject.World * renderer->getPerFrameBuffer()->ViewProj;

		renderer->setPerObjectBuffer(perObject);

		CBPerLine perLine;
		perLine.Color = mLineColor;
		perLine.Width = mLineWidth;
		perLine.PointCount = (int)points.list.size();

		renderer->context()->UpdateSubresource(mpPerLineBuffer, 0, NULL, &perLine, 0, 0);
		renderer->setConstantBuffer(2, mpPerLineBuffer);
		//renderer->setBlendState(true);
		//renderer->setDepthStencilState(D3DRenderer::Depth_Stencil_State_Particle);

		UINT stride = sizeof(Vector3);
		UINT offset = 0;
		UINT indexCount = (UINT)points.list.size() * (mMode == LineDrawMode::LineList ? 2 : 4);

		renderer->context()->IASetVertexBuffers(0, 1, &mpVertexBuffer, &stride, &offset);
		renderer->context()->IASetIndexBuffer(mpIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		renderer->context()->DrawIndexed(indexCount, 0, 0);

		renderer->setBlendState(false);
		renderer->setDepthStencilState(DepthStencilState::Default);
	}
}

}