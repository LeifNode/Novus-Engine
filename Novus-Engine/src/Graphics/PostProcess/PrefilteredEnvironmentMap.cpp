#include "PrefilteredEnvironmentMap.h"
#include "Application/EngineStatics.h"
#include "Graphics/D3DRenderer.h"

namespace novus
{

PrefilteredEnvironmentMap::PrefilteredEnvironmentMap()
	: mpFilterShader(NULL),
	mpSourceTexture(NULL),
	mpFilteredTexture(NULL),
	mpFilteredTextureSRV(NULL),
	mpUAVArray(NULL),
	mpFilterBuffer(NULL)
{}

PrefilteredEnvironmentMap::~PrefilteredEnvironmentMap()
{
	for (int i = 0; i < maxMipFilterLevel * 6; i++)
	{
		ReleaseCOM(mpUAVArray[i]);
	}

	NE_DELETEARR(mpUAVArray);

	ReleaseCOM(mpFilteredTextureSRV);
	ReleaseCOM(mpFilteredTexture);
	ReleaseCOM(mpFilterBuffer);
	NE_DELETE(mpSourceTexture);
}

void PrefilteredEnvironmentMap::Init(D3DRenderer* renderer, const std::wstring& sourcePath)
{
	InitConstantBuffer(renderer);
	InitSourceTexture(renderer, sourcePath);
	InitDestinationTexture(renderer);
	InitFilterShader(renderer);
	FilterSourceTexture(renderer);
}

void PrefilteredEnvironmentMap::Bind(D3DRenderer* renderer, int index)
{
	renderer->context()->CSSetShaderResources(index, 1, &mpFilteredTextureSRV);
}

void PrefilteredEnvironmentMap::InitConstantBuffer(D3DRenderer* renderer)
{
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBFilterEnvMap);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	HR(renderer->device()->CreateBuffer(&bd, NULL, &mpFilterBuffer));
}

void PrefilteredEnvironmentMap::InitSourceTexture(D3DRenderer* renderer, const std::wstring& path)
{
	mpSourceTexture = NE_NEW Texture2D();
	mpSourceTexture->Load(renderer, path.c_str());
}

void PrefilteredEnvironmentMap::InitDestinationTexture(D3DRenderer* renderer)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = mpSourceTexture->getWidth();
	desc.Height = mpSourceTexture->getHeight();
	desc.MipLevels = maxMipFilterLevel;
	desc.ArraySize = 6;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HR(renderer->device()->CreateTexture2D(&desc, 0, &mpFilteredTexture));

	D3D11_UNORDERED_ACCESS_VIEW_DESC rtvDesc;
	rtvDesc.Format = desc.Format;
	rtvDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;

	mpUAVArray = NE_NEW ID3D11UnorderedAccessView*[6 * maxMipFilterLevel];

	for (int i = 0; i < 6; i++)
	{
		for (int mip = 0; mip < maxMipFilterLevel; mip++)
		{
			rtvDesc.Texture2DArray.FirstArraySlice = i;
			rtvDesc.Texture2DArray.MipSlice = mip;
			HR(renderer->device()->CreateUnorderedAccessView(mpFilteredTexture, &rtvDesc, &mpUAVArray[i * maxMipFilterLevel + mip]));
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = maxMipFilterLevel;

	HR(renderer->device()->CreateShaderResourceView(mpFilteredTexture, &srvDesc, &mpFilteredTextureSRV));

	std::string name = "PrefilteredEnvMap";

	mpFilteredTexture->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)name.size(), name.c_str());
}

void PrefilteredEnvironmentMap::InitFilterShader(D3DRenderer* renderer)
{
	mpFilterShader = renderer->getShader("../Shaders/Utils/PrefilterEnvMap.hlsl");

	if (mpFilterShader == NULL)
	{
		ShaderInfo deferredShaderInfo[] = {
			{ ShaderType::Compute, "FilterEnvMapCS" },
			{ ShaderType::None, NULL }
		};

		mpFilterShader = renderer->LoadShader(L"../Shaders/Utils/PrefilterEnvMap.hlsl", deferredShaderInfo, D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, NULL, 0);
	}
}

void PrefilteredEnvironmentMap::FilterSourceTexture(D3DRenderer* renderer)
{
	renderer->setShader(mpFilterShader);
	renderer->setTextureResource(0, mpSourceTexture);
	renderer->ResetSamplerState();

	CBFilterEnvMap filterBuffer;
	filterBuffer.MipCount = maxMipFilterLevel;

	Vector3 targets[6] =
	{
		Vector3(1.0f, 0.0, 0.0), // +X
		Vector3(-1.0f, 0.0, 0.0), // -X
		Vector3(0.0, 1.0f, 0.0), // +Y
		Vector3(0.0, -1.0f, 0.0), // -Y
		Vector3(0.0, 0.0, 1.0f), // +Z
		Vector3(0.0, 0.0, -1.0f)  // -Z
	};

	Vector3 ups[6] =
	{
		Vector3(0.0f, 1.0f, 0.0f),  // +X
		Vector3(0.0f, 1.0f, 0.0f),  // -X
		Vector3(0.0f, 0.0f, -1.0f), // +Y
		Vector3(0.0f, 0.0f, 1.0f),  // -Y
		Vector3(0.0f, 1.0f, 0.0f),	 // +Z
		Vector3(0.0f, 1.0f, 0.0f)	 // -Z
	};
	
	//Loop through cube faces
	for (int i = 0; i < 6; i++)
	{
		Vector2i dim = Vector2i(mpSourceTexture->getWidth(), mpSourceTexture->getHeight());

		filterBuffer.Forward = targets[i];
		filterBuffer.Up = ups[i];

		//Loop through mip levels
		//TODO: determine maxMipFilterLevel dynamically so I don't assume 1028x1028 textures
		for (int mip = 0; mip < maxMipFilterLevel; mip++)
		{
			filterBuffer.SourceDimensions = dim;
			filterBuffer.CurrentMip = maxMipFilterLevel - mip;

			//std::cout << "Current Mip: " << mip + 2 << ", Corresponding roughness: " << powf(2.0f, (maxMipFilterLevel - 4.0f - mip - 2) / 1.15f) << std::endl;

			renderer->context()->UpdateSubresource(mpFilterBuffer, 0, NULL, &filterBuffer, 0, 0);
			renderer->setConstantBuffer(2, mpFilterBuffer);
			renderer->context()->CSSetUnorderedAccessViews(0, 1, &mpUAVArray[i * maxMipFilterLevel + mip], 0);

			renderer->context()->Dispatch(filterBuffer.SourceDimensions.x / 16, filterBuffer.SourceDimensions.y / 16, 1);

			std::stringstream stream;
			stream << "Dispatched mip " << mip << " for face " << i;

			NE_MESSAGE(stream.str().c_str(), "PrefilteredEnvMap");

			dim /= 2;
		}
	}
}

}