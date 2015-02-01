//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#include "Application/Common.h"

namespace novus
{

class D3DRenderer;

template <class T>
class MeshRenderer
{
public:
	MeshRenderer();
	~MeshRenderer();

	void Init(std::vector<T>& vertices, std::vector<UINT>& indices, D3DRenderer* renderer);

	void Render(D3DRenderer* renderer);

private:
	ID3D11Buffer* mpVB;
	ID3D11Buffer* mpIB;

	int mIndexCount;
};


template <class T>
MeshRenderer<T>::MeshRenderer()
:mpVB(nullptr),
mpIB(nullptr),
mIndexCount(0)
{
}

template <class T>
MeshRenderer<T>::~MeshRenderer()
{
	ReleaseCOM(mpVB);
	ReleaseCOM(mpIB);
	mIndexCount = 0;
}

template <class T>
void MeshRenderer<T>::Init(std::vector<T>& vertices, std::vector<UINT>& indices, D3DRenderer* renderer)
{
	ReleaseCOM(mpVB);
	ReleaseCOM(mpIB);

	mIndexCount = (int)indices.size();

	//Vertex Buffer
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(T)* (UINT)vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &vertices[0];

	HR(renderer->device()->CreateBuffer(&bd, &initData, &mpVB));

	//Index Buffer
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* (UINT)indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];

	HR(renderer->device()->CreateBuffer(&ibd, &iinitData, &mpIB));
}

template <class T>
void MeshRenderer<T>::Render(D3DRenderer* renderer)
{
	ID3D11DeviceContext* context = renderer->context();

	UINT stride = sizeof(T);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &mpVB, &stride, &offset);
	context->IASetIndexBuffer(mpIB, DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(mIndexCount, 0, 0);
}

}
