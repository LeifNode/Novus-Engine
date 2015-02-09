//#include "LineRenderer.h"
//#include "Utils/Primitives/LineSegment.h"
//#include "D3DRenderer.h"
//
//namespace novus
//{
//
//Shader* LineRenderer::pLineShader = NULL;
//ID3D11Buffer* LineRenderer::pPerLineBuffer = NULL;
//
//LineRenderer::LineRenderer()
//	:mpVertexBuffer(NULL),
//	mpIndexBuffer(NULL)
//{
//	mMode = LINE_DRAW_MODE_LINELIST;
//	mLineThickness = 0.01f;
//	mLineColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
//}
//
//LineRenderer::~LineRenderer()
//{
//	ReleaseCOM(mpVertexBuffer);
//	ReleaseCOM(mpIndexBuffer);
//}
//
//void LineRenderer::setDrawMode(LineRenderer::LINE_DRAW_MODE mode)
//{
//	if (mMode != mode)
//	{
//		mMode = mode;
//		reloadPoints();
//	}
//}
//
//void LineRenderer::setLineThickness(float thickness)
//{
//	mLineThickness = thickness;
//}
//
//void LineRenderer::setLineColor(const XMFLOAT4& color)
//{
//	mLineColor = color;
//}
//
//void LineRenderer::generateIndices(UINT** ppArrayOut, UINT* pIndexCount)
//{
//	//Line list = 2 indices per point, Line strip = 4 indices per point
//	UINT indexCount = (UINT)Points.List.size() * (mMode == LINE_DRAW_MODE_LINELIST ? 2 : 4);
//	*pIndexCount = indexCount;
//
//	UINT* indexArray = LE_NEW UINT[indexCount];
//
//	//Line List indexing
//	if (mMode == LINE_DRAW_MODE_LINELIST)
//	{
//		if (Points.List.size() > 1)
//		{
//			for (int i = 0; i < Points.List.size() - 1; i += 2)
//			{
//				if (i > 0)
//					indexArray[i * 2] = i - 2; //Change to 2 for smoothing
//				else
//					indexArray[i * 2] = i;
//
//				indexArray[i * 2 + 1] = i;
//				indexArray[i * 2 + 2] = i + 1;
//
//				if (i < Points.List.size() - 2)
//					indexArray[i * 2 + 3] = i + 3; //Change to 3 for smoothing
//				else
//					indexArray[i * 2 + 3] = i + 1;
//			}
//		}
//	}
//	else if (mMode == LINE_DRAW_MODE_LINESTRIP)
//	{
//		//Line strip indexing
//		if (Points.List.size() > 1)
//		{
//			for (int i = 0; i < Points.List.size() - 1; i++)
//			{
//				if (i > 0)
//					indexArray[i * 4] = i - 1;
//				else
//					indexArray[i * 4] = i;
//
//				indexArray[i * 4 + 1] = i;
//				indexArray[i * 4 + 2] = i + 1;
//
//				if (i < Points.List.size() - 2)
//					indexArray[i * 4 + 3] = i + 2;
//				else
//					indexArray[i * 4 + 3] = i + 1;
//			}
//		}
//	}
//
//	*ppArrayOut = indexArray;
//}
//
//void LineRenderer::reloadPoints()
//{
//	if (Points.List.size() > 0)
//	{
//		ReleaseCOM(mpVertexBuffer);
//		ReleaseCOM(mpIndexBuffer);
//
//		UINT* indexArr;
//		UINT indexCount;
//
//		generateIndices(&indexArr, &indexCount);
//
//		D3D11_BUFFER_DESC bd;
//		bd.Usage = D3D11_USAGE_IMMUTABLE;
//		bd.ByteWidth = sizeof(XMFLOAT3)* (UINT)Points.List.size();
//		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		bd.CPUAccessFlags = 0;
//		bd.MiscFlags = 0;
//		bd.StructureByteStride = 0;
//
//		D3D11_SUBRESOURCE_DATA initData;
//		initData.pSysMem = &Points.List[0];
//
//		HR(gpApplication->getRenderer()->device()->CreateBuffer(&bd, &initData, &mpVertexBuffer));
//
//		D3D11_BUFFER_DESC ibd;
//		ibd.Usage = D3D11_USAGE_IMMUTABLE;
//		ibd.ByteWidth = sizeof(UINT)* indexCount;
//		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//		ibd.CPUAccessFlags = 0;
//		ibd.MiscFlags = 0;
//		ibd.StructureByteStride = 0;
//		D3D11_SUBRESOURCE_DATA iinitData;
//
//		iinitData.pSysMem = &indexArr[0];
//
//		HR(gpApplication->getRenderer()->device()->CreateBuffer(&ibd, &iinitData, &mpIndexBuffer));
//
//		SAFE_DELETEARR(indexArr);
//	}
//}
//
//void LineRenderer::Initialize()
//{
//	D3DRenderer* renderer = gpApplication->getRenderer();
//
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
//	};
//
//	pLineShader = renderer->loadShader(L"Shaders/line.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ, vertexDescription, ARRAYSIZE(vertexDescription));
//
//	D3D11_BUFFER_DESC bd;
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = sizeof(CBPerLine);
//	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	bd.CPUAccessFlags = 0;
//	bd.MiscFlags = 0;
//	bd.StructureByteStride = 0;
//
//	renderer->device()->CreateBuffer(&bd, NULL, &pPerLineBuffer);
//}
//
//void LineRenderer::DeInit()
//{
//	ReleaseCOM(pPerLineBuffer);
//}
//
//void LineRenderer::Render(D3DRenderer* renderer)
//{
//	if (mpVertexBuffer != NULL)
//	{
//		renderer->setShader(pLineShader);
//
//		CBPerFrame perFrame = *renderer->getPerFrameBuffer();
//		renderer->setPerFrameBuffer(perFrame);
//
//		CBPerObject perObject;
//		perObject.World = XMMatrixIdentity();
//		perObject.WorldInvTranspose = XMMatrixInverse(NULL, XMMatrixTranspose(perObject.World));
//		perObject.WorldViewProj = perObject.World * renderer->getPerFrameBuffer()->ViewProj;
//
//		renderer->setPerObjectBuffer(perObject);
//
//		CBPerLine perLine;
//		perLine.Color = mLineColor;
//		perLine.Thickness = mLineThickness;
//		perLine.PointCount = (int)Points.List.size();
//
//		renderer->context()->UpdateSubresource(pPerLineBuffer, 0, NULL, &perLine, 0, 0);
//		renderer->setConstantBuffer(2, pPerLineBuffer);
//		//renderer->setBlendState(true);
//		//renderer->setDepthStencilState(D3DRenderer::Depth_Stencil_State_Particle);
//
//		UINT stride = sizeof(XMFLOAT3);
//		UINT offset = 0;
//		UINT indexCount = (UINT)Points.List.size() * (mMode == LINE_DRAW_MODE_LINELIST ? 2 : 4);
//
//		renderer->context()->IASetVertexBuffers(0, 1, &mpVertexBuffer, &stride, &offset);
//		renderer->context()->IASetIndexBuffer(mpIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
//
//		renderer->context()->DrawIndexed(indexCount, 0, 0);
//
//		renderer->setBlendState(false);
//		renderer->setDepthStencilState(D3DRenderer::Depth_Stencil_State_Default);
//	}
//}
//
//}