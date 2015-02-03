//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_STRUCTURED_BUFFER_H
#define NOVUS_STRUCTURED_BUFFER_H

#include "Application/Common.h"
#include "D3DRenderer.h"

namespace novus
{

template <typename T>
class StructuredBuffer
{
public:
	StructuredBuffer();
	~StructuredBuffer();

	void Init(D3DRenderer* renderer,
			  int elements,
			  UINT bindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
			  bool dynamic = false);

	T* Map(D3DRenderer* renderer);
	void Unmap(D3DRenderer* renderer);

	int getSize() const { return mSize; }

	ID3D11Buffer* getBuffer() const { return mpBuffer; }
	ID3D11ShaderResourceView* getSRV() const { return mpBufferSRV; }
	ID3D11UnorderedAccessView* getUAV() const { return mpBufferUAV; }

private:
	StructuredBuffer(const StructuredBuffer<T>&);
	StructuredBuffer<T>& operator= (const StructuredBuffer<T>&);

private:
	int mSize;

	ID3D11Buffer* mpBuffer;
	ID3D11ShaderResourceView* mpBufferSRV;
	ID3D11UnorderedAccessView* mpBufferUAV;
};

template <typename T>
StructuredBuffer<T>::StructuredBuffer()
	: mpBuffer(NULL),
	mpBufferSRV(NULL),
	mpBufferUAV(NULL),
	mSize(0)
{
}

template <typename T>
StructuredBuffer<T>::~StructuredBuffer()
{
	ReleaseCOM(mpBufferSRV);
	ReleaseCOM(mpBufferUAV);
	ReleaseCOM(mpBuffer);
}

template <typename T>
void StructuredBuffer<T>::Init(D3DRenderer* renderer, int elements, UINT bindFlags, bool dynamic)
{
	ReleaseCOM(mpBufferSRV);
	ReleaseCOM(mpBufferUAV);
	ReleaseCOM(mpBuffer);

	mSize = elements;

	D3D11_BUFFER_DESC desc;

	desc.ByteWidth = sizeof(T) * elements;
	desc.BindFlags = bindFlags;
	desc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(T);

	HR(renderer->device()->CreateBuffer(&desc, NULL, &mpBuffer));

	if (bindFlags & D3D11_BIND_SHADER_RESOURCE)
		HR(renderer->device()->CreateShaderResourceView(mpBuffer, NULL, &mpBufferSRV));

	if (bindFlags & D3D11_BIND_UNORDERED_ACCESS)
		HR(renderer->device()->CreateUnorderedAccessView(mpBuffer, NULL, &mpBufferUAV));
}

template <typename T>
T* StructuredBuffer<T>::Map(D3DRenderer* renderer)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	renderer->context()->Map(mpBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	return static_cast<T>(resource.pData);
}

template <typename T>
void StructuredBuffer<T>::Unmap(D3DRenderer* renderer)
{
	renderer->context()->Unmap(mpBuffer, 0);
}

}

#endif