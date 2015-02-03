#include "D3DLocalInclude.h"

namespace novus
{

D3DLocalInclude::D3DLocalInclude(const char* shaderRootPath)
	: mShaderRootPath(shaderRootPath)
{
}

HRESULT __stdcall D3DLocalInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
	char fullPath[256] = "";
	strcat_s(fullPath, mShaderRootPath.c_str());
	strcat_s(fullPath, "/");
	strcat_s(fullPath, pFileName);

	std::ifstream include;
	include.open(fullPath, std::ios::in | std::ios::binary | std::ios::ate);

	if (!include.is_open())
	{
		/*char message[256] = "";
		strcat_s(message, "Could not open include file: ");
		strcat_s(message, fullPath);
		strcat_s(message, "\n");
		OutputDebugStringA(message);*/

		return E_FAIL;
	}

	size_t includeSize = static_cast<size_t>(include.tellg());
	*pBytes = static_cast<UINT>(includeSize);
	char* includeMem = NE_NEW char[includeSize];

	include.seekg(0, std::ios::beg);
	include.read(includeMem, includeSize);
	include.close();

	*ppData = includeMem;

	return S_OK;
}

HRESULT __stdcall D3DLocalInclude::Close(LPCVOID pData)
{
	char* includeMem = (char*)pData;

	NE_DELETEARR(includeMem);

	return S_OK;
}

}