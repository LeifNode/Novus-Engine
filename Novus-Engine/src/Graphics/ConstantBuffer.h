//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_CONSTANT_BUFFER_H
#define NOVUS_CONSTANT_BUFFER_H

#include "Application/Common.h"
#include "D3DRenderer.h"

namespace novus
{

class D3DRenderer;

template <typename T>
class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	void Init(D3DRenderer* renderer);

	T* Map(D3DRenderer* renderer);
	void Unmap(D3DRenderer* renderer);

	void Bind(int index, D3DRenderer* renderer);

private:
	ID3D11Buffer* mpBuffer;
};

template <typename T>
ConstantBuffer<T>::ConstantBuffer()
:mpBuffer(NULL)
{
}

template <typename T>
ConstantBuffer<T>::~ConstantBuffer()
{
	ReleaseCOM(mpBuffer);
}

template <typename T>
void ConstantBuffer<T>::Init(D3DRenderer* renderer)
{
	ReleaseCOM(mpBuffer);

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(T);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	HR(renderer->device()->CreateBuffer(&bd, NULL, &mpBuffer));
}

template <typename T>
T* ConstantBuffer<T>::Map(D3DRenderer* renderer)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	HR(renderer->context()->Map(mpBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource));
	T* constantBuffer = static_cast<T*>(resource.pData);

	return constantBuffer;
}

template <typename T>
void ConstantBuffer<T>::Unmap(D3DRenderer* renderer)
{
	renderer->context()->Unmap(mpBuffer, 0);
}

template <typename T>
void ConstantBuffer<T>::Bind(int index, D3DRenderer* renderer)
{
	renderer->setConstantBuffer(index, mpBuffer);
}

}

#endif