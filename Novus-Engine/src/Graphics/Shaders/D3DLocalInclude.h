//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_D3D_LOCAL_INCLUDE_H
#define NOVUS_D3D_LOCAL_INCLUDE_H

#include "Application/Common.h"

namespace novus
{

class D3DLocalInclude : public ID3DInclude
{
public:
	D3DLocalInclude(const char* shaderRootPath);

	virtual HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) override;
	virtual HRESULT __stdcall Close(LPCVOID pData) override;

private:
	std::string mShaderRootPath;
};

}

#endif