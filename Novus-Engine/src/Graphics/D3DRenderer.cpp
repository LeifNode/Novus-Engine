#include "Graphics/D3DRenderer.h"
#include "Application/EngineStatics.h"
#include "Application/NovusApplication.h"
#include "Math/Transform.h"

namespace novus
{

D3DRenderer::D3DRenderer()
	:
	md3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
	mEnable4xMsaa(false),
	m4xMsaaQuality(16),

	mpd3dDevice(NULL),
	mpd3dImmediateContext(NULL),
	mpd3dDebug(NULL),
	mpSwapChain(NULL),
	mpDepthStencilBuffer(NULL),
	mpRenderTarget(NULL),
	mpDepthStencilView(NULL),
	mpPerFrameBuffer(NULL),
	mpPerObjectBuffer(NULL),
	mpBlendStateAlpha(NULL),
	mpBlendStateOpaque(NULL),
	mpGBuffer(NULL),
	//mpDeferredRenderer(NULL),
	//mpOVRManager(NULL),
	mUseHMD(false)
{
	/*mClearColor[0] = 0.0f;
	mClearColor[1] = 0.125f;
	mClearColor[2] = 0.3f;
	mClearColor[3] = 1.0f;*/

	mClearColor[0] = 0.0f;
	mClearColor[1] = 0.0f;
	mClearColor[2] = 0.0f;
	mClearColor[3] = 1.0f;

	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));
	ZeroMemory(&mpDepthStencilStates[0], sizeof(ID3D11DepthStencilState*)* DepthStencilState::COUNT);

	//mpOVRManager = NE_NEW OVRManager();
}

D3DRenderer::~D3DRenderer()
{
	NE_DELETE(mpGBuffer);
	//NE_DELETE(mpDeferredRenderer);
	//NE_DELETE(mpOVRManager);

	for (auto it = mLoadedShaders.begin(); it != mLoadedShaders.end(); ++it)
	{
		NE_DELETE(it->second);
	}
	mLoadedShaders.clear();

	for (int i = 0; i < DepthStencilState::COUNT; i++)
	{
		ReleaseCOM(mpDepthStencilStates[i]);
	}

	ReleaseCOM(mpBlendStateAlpha);
	ReleaseCOM(mpBlendStateOpaque);
	ReleaseCOM(mpSamplerState);
	ReleaseCOM(mpPerFrameBuffer);
	ReleaseCOM(mpPerObjectBuffer);
	ReleaseCOM(mpRenderTarget);
	ReleaseCOM(mpDepthStencilView);
	ReleaseCOM(mpSwapChain);
	ReleaseCOM(mpDepthStencilBuffer);

	if (mpd3dImmediateContext)
		mpd3dImmediateContext->ClearState();

	if (mpd3dDebug != NULL)
	{
		mpd3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		ReleaseCOM(mpd3dDebug);
	}

	ReleaseCOM(mpd3dImmediateContext);
	ReleaseCOM(mpd3dDevice);
}

void D3DRenderer::OnResize()
{
	int width = 0, height = 0;

	if (isUsingHMD())
	{
		//OVR::Sizei textureSize = mpOVRManager->getRenderTargetSize();
		width = 1920;
		height = 1080;
	}
	else
	{
		width = EngineStatics::getApplication()->getClientWidth();
		height = EngineStatics::getApplication()->getClientHeight();
	}

	assert(mpd3dImmediateContext);
	assert(mpd3dDevice);
	assert(mpSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	ReleaseCOM(mpRenderTarget);
	ReleaseCOM(mpDepthStencilView);
	ReleaseCOM(mpDepthStencilBuffer);


	// Resize the swap chain and recreate the render target view.

	HR(mpSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(mpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(mpd3dDevice->CreateRenderTargetView(backBuffer, 0, &mpRenderTarget));
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//depthStencilDesc.Format    = DXGI_FORMAT_D32_FLOAT;

	// Use 4X MSAA? --must match swap chain MSAA values.
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(mpd3dDevice->CreateTexture2D(&depthStencilDesc, 0, &mpDepthStencilBuffer));
	HR(mpd3dDevice->CreateDepthStencilView(mpDepthStencilBuffer, 0, &mpDepthStencilView));


	// Bind the render target view and depth/stencil view to the pipeline.

	mpd3dImmediateContext->OMSetRenderTargets(1, &mpRenderTarget, mpDepthStencilView);


	// Set the viewport transform.

	setViewport(0, 0, width, height);

	//mpOVRManager->OnResize();

	mpGBuffer->OnResize(width, height);
}

bool D3DRenderer::Init()
{
	// Create the device and device context.

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,                 // default adapter
		md3dDriverType,
		0,                 // no software device
		createDeviceFlags,
		0, 0,              // default feature level array
		D3D11_SDK_VERSION,
		&mpd3dDevice,
		&featureLevel,
		&mpd3dImmediateContext);

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	HR(mpd3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
	assert(m4xMsaaQuality > 0);

	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferDesc.Width = EngineStatics::getApplication()->getClientWidth();
	sd.BufferDesc.Height = EngineStatics::getApplication()->getClientHeight();
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Use 4X MSAA? 
	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = EngineStatics::getApplication()->getMainWnd();
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// To correctly create the swap chain, we must use the IDXGIFactory that was
	// used to create the device.  If we tried to use a different IDXGIFactory instance
	// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
	// This function is being called with a device from a different IDXGIFactory."

	IDXGIDevice* dxgiDevice = 0;
	HR(mpd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(mpd3dDevice, &sd, &mpSwapChain));

#if defined(DEBUG) || defined(_DEBUG)
	HR(mpd3dDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&mpd3dDebug));
#endif

	//IDXGIOutput* output;

	//dxgiAdapter->EnumOutputs(2, &output);

	//DXGI_OUTPUT_DESC outputDesc;

	//output->GetDesc(&outputDesc);
	//output->GetDisplaySurfaceData
	//std::cout << "Monitor: " << outputDesc.DesktopCoordinates.left << std::endl;

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	// The remaining steps that need to be carried out for d3d creation
	// also need to be executed every time the window is resized.  So
	// just call the OnResize method here to avoid code duplication.

	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = true;

	ID3D11RasterizerState* rasterState;

	mpd3dDevice->CreateRasterizerState(&rasterDesc, &rasterState);
	mpd3dImmediateContext->RSSetState(rasterState);

	ReleaseCOM(rasterState);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	mpd3dDevice->CreateBlendState(&blendDesc, &mpBlendStateAlpha);

	blendDesc.RenderTarget[0].BlendEnable = FALSE;

	mpd3dDevice->CreateBlendState(&blendDesc, &mpBlendStateOpaque);

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;

	context()->OMSetBlendState(mpBlendStateOpaque, blendFactor, sampleMask);

	//Per frame buffer
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBPerFrame);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	mpd3dDevice->CreateBuffer(&bd, NULL, &mpPerFrameBuffer);

	//Per object buffer
	bd.ByteWidth = sizeof(CBPerObject);

	mpd3dDevice->CreateBuffer(&bd, NULL, &mpPerObjectBuffer);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 4;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	mpd3dDevice->CreateSamplerState(&sampDesc, &mpSamplerState);

	mpGBuffer = NE_NEW GBuffer();
	mpGBuffer->Init(EngineStatics::getApplication()->getClientWidth(), EngineStatics::getApplication()->getClientHeight());

	//mpDeferredRenderer = NE_NEW DeferredRenderer();
	//mpDeferredRenderer->Initialize();

	OnResize();

	//mpOVRManager->Initialize();

	InitDepthStencilStates();

	return true;
}

void D3DRenderer::InitDepthStencilStates()
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	//Default state
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	mpd3dDevice->CreateDepthStencilState(&dsDesc, &mpDepthStencilStates[DepthStencilState::Default]);

	dsDesc.DepthEnable = true;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	mpd3dDevice->CreateDepthStencilState(&dsDesc, &mpDepthStencilStates[DepthStencilState::Particle]);

	mpd3dImmediateContext->OMSetDepthStencilState(mpDepthStencilStates[DepthStencilState::Default], 0);
}

HRESULT D3DRenderer::CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, const D3D_SHADER_MACRO* defines, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DCompileFromFile(szFileName, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

Shader* D3DRenderer::LoadShaderUnmanaged(const WCHAR* filePath,
	const ShaderInfo* shaderInfo,
	D3D_PRIMITIVE_TOPOLOGY primitiveTopology,
	const D3D11_INPUT_ELEMENT_DESC* vertexDescription,
	int vertexDescriptionSize,
	const D3D_SHADER_MACRO* defines)
{
	char* name = NE_NEW char[MAX_PATH];

	//Convert wchar* to char*
	wcstombs(name, filePath, MAX_PATH);

	std::string nameStr(name);

	NE_DELETEARR(name);
	name = nullptr;

	Shader* newShader = NE_NEW Shader(nameStr);
	newShader->mPrimitiveTopology = primitiveTopology;

	HRESULT hr = S_OK;
	ID3DBlob* shaderBlob = nullptr;

	for (int i = 0; shaderInfo[i].type != ShaderType::None; i++)
	{
		shaderBlob = nullptr;

		switch (shaderInfo[i].type)
		{
		case ShaderType::Vertex:
			hr = CompileShaderFromFile(filePath, shaderInfo[i].entrypoint, "vs_5_0", defines, &shaderBlob);
			if (FAILED(hr))
			{
				MessageBox(0, L"The FX file cannot be compiled.", L"Error", MB_OK);

				return NULL;
			}

			hr = device()->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &newShader->mpVertexShader);
			if (FAILED(hr))
			{
				shaderBlob->Release();

				MessageBox(0, L"Unable to create vertex shader.", L"Error", MB_OK);
			}

			//Create the input layout
			if (vertexDescription != NULL && vertexDescriptionSize > 0)
				hr = mpd3dDevice->CreateInputLayout(vertexDescription, vertexDescriptionSize, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &newShader->mpInputLayout);
			else
				newShader->mpInputLayout = NULL;

			break;
		case ShaderType::Pixel:
			hr = CompileShaderFromFile(filePath, shaderInfo[i].entrypoint, "ps_5_0", defines, &shaderBlob);
			if (FAILED(hr))
			{
				MessageBox(0, L"The FX file cannot be compiled.", L"Error", MB_OK);

				return NULL;
			}

			hr = device()->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &newShader->mpPixelShader);
			if (FAILED(hr))
			{
				shaderBlob->Release();

				MessageBox(0, L"Unable to create pixel shader.", L"Error", MB_OK);
			}
			break;
		case ShaderType::Geometry:
			hr = CompileShaderFromFile(filePath, shaderInfo[i].entrypoint, "gs_5_0", defines, &shaderBlob);
			if (FAILED(hr))
			{
				MessageBox(0, L"The FX file cannot be compiled.", L"Error", MB_OK);

				return NULL;
			}

			hr = device()->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &newShader->mpGeometryShader);
			if (FAILED(hr))
			{
				shaderBlob->Release();

				MessageBox(0, L"Unable to create geometry shader.", L"Error", MB_OK);
			}
			break;
		case ShaderType::Compute:
			hr = CompileShaderFromFile(filePath, shaderInfo[i].entrypoint, "cs_5_0", defines, &shaderBlob);
			if (FAILED(hr))
			{
				MessageBox(0, L"The FX file cannot be compiled.", L"Error", MB_OK);

				return NULL;
			}

			hr = device()->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &newShader->mpComputeShader);
			if (FAILED(hr))
			{
				shaderBlob->Release();

				MessageBox(0, L"Unable to create compute shader.", L"Error", MB_OK);
			}
			break;
		case ShaderType::Hull:
			hr = CompileShaderFromFile(filePath, shaderInfo[i].entrypoint, "hs_5_0", defines, &shaderBlob);
			if (FAILED(hr))
			{
				MessageBox(0, L"The FX file cannot be compiled.", L"Error", MB_OK);

				return NULL;
			}

			hr = device()->CreateHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &newShader->mpHullShader);
			if (FAILED(hr))
			{
				shaderBlob->Release();

				MessageBox(0, L"Unable to create hull shader.", L"Error", MB_OK);
			}
			break;
		case ShaderType::Domain:
			hr = CompileShaderFromFile(filePath, shaderInfo[i].entrypoint, "ds_5_0", defines, &shaderBlob);
			if (FAILED(hr))
			{
				MessageBox(0, L"The FX file cannot be compiled.", L"Error", MB_OK);

				return NULL;
			}

			hr = device()->CreateDomainShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &newShader->mpDomainShader);
			if (FAILED(hr))
			{
				shaderBlob->Release();

				MessageBox(0, L"Unable to create domain shader.", L"Error", MB_OK);
			}
			break;
		}

		ReleaseCOM(shaderBlob);
	}

	return newShader;
}

Shader* D3DRenderer::LoadShader(const WCHAR* filePath, const ShaderInfo* shaderInfo, D3D_PRIMITIVE_TOPOLOGY primitiveTopology, const D3D11_INPUT_ELEMENT_DESC* vertexDescription, int vertexDescriptionSize, const D3D_SHADER_MACRO* defines)
{
	char* name = NE_NEW char[MAX_PATH];

	//Convert wchar* to char*
	wcstombs(name, filePath, MAX_PATH);

	std::string nameStr(name);

	NE_DELETEARR(name);
	name = nullptr;

	Shader* newShader = LoadShaderUnmanaged(filePath, shaderInfo, primitiveTopology, vertexDescription, vertexDescriptionSize, defines);

	mLoadedShaders[nameStr] = newShader;

	return newShader;
}

void D3DRenderer::setTextureResource(int index, Texture* texture)
{
	if (mpActiveShader)
	{
		if (mpActiveShader->hasVertexShader())
			context()->VSSetShaderResources(index, 1, &texture->mpResourceView);
		if (mpActiveShader->hasPixelShader())
			context()->PSSetShaderResources(index, 1, &texture->mpResourceView);
		if (mpActiveShader->hasGeometryShader())
			context()->GSSetShaderResources(index, 1, &texture->mpResourceView);
		if (mpActiveShader->hasComputeShader())
			context()->CSSetShaderResources(index, 1, &texture->mpResourceView);
		if (mpActiveShader->hasHullShader())
			context()->HSSetShaderResources(index, 1, &texture->mpResourceView);
		if (mpActiveShader->hasDomainShader())
			context()->DSSetShaderResources(index, 1, &texture->mpResourceView);
	}
}

void D3DRenderer::setTextureResources(Texture** texArray, int startSlot, unsigned count)
{
	if (mpActiveShader)
	{
		ID3D11ShaderResourceView** resourceArray = NE_NEW ID3D11ShaderResourceView*[count];

		for (unsigned int i = 0; i < count; i++)
			resourceArray[i] = texArray[i]->mpResourceView;

		if (mpActiveShader->hasVertexShader())
			mpd3dImmediateContext->VSSetShaderResources(startSlot, count, resourceArray);
		if (mpActiveShader->hasPixelShader())
			mpd3dImmediateContext->PSSetShaderResources(startSlot, count, resourceArray);
		if (mpActiveShader->hasGeometryShader())
			mpd3dImmediateContext->GSSetShaderResources(startSlot, count, resourceArray);
		if (mpActiveShader->hasComputeShader())
			mpd3dImmediateContext->CSSetShaderResources(startSlot, count, resourceArray);
		if (mpActiveShader->hasHullShader())
			mpd3dImmediateContext->HSSetShaderResources(startSlot, count, resourceArray);
		if (mpActiveShader->hasDomainShader())
			mpd3dImmediateContext->DSSetShaderResources(startSlot, count, resourceArray);

		NE_DELETEARR(resourceArray);
	}
}

void D3DRenderer::setTextureResources(ID3D11ShaderResourceView** texArray, int startSlot, unsigned count)
{
	if (mpActiveShader)
	{
		if (mpActiveShader->hasVertexShader())
			mpd3dImmediateContext->VSSetShaderResources(startSlot, count, texArray);
		if (mpActiveShader->hasPixelShader())
			mpd3dImmediateContext->PSSetShaderResources(startSlot, count, texArray);
		if (mpActiveShader->hasGeometryShader())
			mpd3dImmediateContext->GSSetShaderResources(startSlot, count, texArray);
		if (mpActiveShader->hasComputeShader())
			mpd3dImmediateContext->CSSetShaderResources(startSlot, count, texArray);
		if (mpActiveShader->hasHullShader())
			mpd3dImmediateContext->HSSetShaderResources(startSlot, count, texArray);
		if (mpActiveShader->hasDomainShader())
			mpd3dImmediateContext->DSSetShaderResources(startSlot, count, texArray);
	}
}

void D3DRenderer::UnbindTextureResources()
{
	ID3D11ShaderResourceView** arr = NE_NEW ID3D11ShaderResourceView*[8];
	for (int i = 0; i < 8; i++) arr[i] = NULL;

	mpd3dImmediateContext->VSSetShaderResources(0, 8, arr);
	mpd3dImmediateContext->PSSetShaderResources(0, 8, arr);
	mpd3dImmediateContext->GSSetShaderResources(0, 8, arr);
	mpd3dImmediateContext->CSSetShaderResources(0, 8, arr);
	mpd3dImmediateContext->HSSetShaderResources(0, 8, arr);
	mpd3dImmediateContext->DSSetShaderResources(0, 8, arr);

	NE_DELETEARR(arr);
}

void D3DRenderer::setSampler(int index, ID3D11SamplerState* samplerState)
{
	if (mpActiveShader)
	{
		if (mpActiveShader->hasVertexShader())
			mpd3dImmediateContext->VSSetSamplers(index, 1, &samplerState);
		if (mpActiveShader->hasPixelShader())
			mpd3dImmediateContext->PSSetSamplers(index, 1, &samplerState);
		if (mpActiveShader->hasGeometryShader())
			mpd3dImmediateContext->GSSetSamplers(index, 1, &samplerState);
		if (mpActiveShader->hasComputeShader())
			mpd3dImmediateContext->CSSetSamplers(index, 1, &samplerState);
		if (mpActiveShader->hasHullShader())
			mpd3dImmediateContext->HSSetSamplers(index, 1, &samplerState);
		if (mpActiveShader->hasDomainShader())
			mpd3dImmediateContext->DSSetSamplers(index, 1, &samplerState);
	}
}

void D3DRenderer::setConstantBuffer(int index, ID3D11Buffer* buffer)
{
	if (mpActiveShader)
	{
		if (mpActiveShader->hasVertexShader())
			mpd3dImmediateContext->VSSetConstantBuffers(index, 1, &buffer);
		if (mpActiveShader->hasPixelShader())
			mpd3dImmediateContext->PSSetConstantBuffers(index, 1, &buffer);
		if (mpActiveShader->hasGeometryShader())
			mpd3dImmediateContext->GSSetConstantBuffers(index, 1, &buffer);
		if (mpActiveShader->hasComputeShader())
			mpd3dImmediateContext->CSSetConstantBuffers(index, 1, &buffer);
		if (mpActiveShader->hasHullShader())
			mpd3dImmediateContext->HSSetConstantBuffers(index, 1, &buffer);
		if (mpActiveShader->hasDomainShader())
			mpd3dImmediateContext->DSSetConstantBuffers(index, 1, &buffer);
	}
}

void D3DRenderer::setPerFrameBuffer(CBPerFrame& buffer)
{
	///TODO: use map with discard flag instead of UpdateSubresource
	mpd3dImmediateContext->UpdateSubresource(mpPerFrameBuffer, 0, NULL, &buffer, 0, 0);
	setConstantBuffer(0, mpPerFrameBuffer);
	mPerFrameData = buffer;
}

void D3DRenderer::setPerObjectBuffer(CBPerObject& buffer)
{
	///TODO: use map with discard flag instead of UpdateSubresource
	mpd3dImmediateContext->UpdateSubresource(mpPerObjectBuffer, 0, NULL, &buffer, 0, 0);
	setConstantBuffer(1, mpPerObjectBuffer);
}

void D3DRenderer::bindPerFrameBuffer()
{
	setConstantBuffer(0, mpPerFrameBuffer);
}

Texture* D3DRenderer::CreateTexture(D3D11_TEXTURE2D_DESC* textureDescription, DXGI_FORMAT resViewFmt)
{
	Texture* newTexture = NE_NEW Texture();
	newTexture->mWidth = textureDescription->Width;
	newTexture->mHeight = textureDescription->Height;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	if (resViewFmt == DXGI_FORMAT_UNKNOWN)
		shaderResourceViewDesc.Format = textureDescription->Format;
	else
		shaderResourceViewDesc.Format = resViewFmt;

	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = textureDescription->MipLevels;

	HRESULT result = mpd3dDevice->CreateTexture2D(textureDescription, NULL, &newTexture->mpTexture);
	if (FAILED(result))
	{
		NE_DELETE(newTexture);
		return NULL;
	}

	result = mpd3dDevice->CreateShaderResourceView(newTexture->mpTexture, &shaderResourceViewDesc, &newTexture->mpResourceView);
	if (FAILED(result))
	{
		NE_DELETE(newTexture);
		return NULL;
	}

	return newTexture;
}

bool D3DRenderer::CreateTexture(D3D11_TEXTURE2D_DESC* textureDescription, ID3D11Texture2D** textureOut, ID3D11ShaderResourceView** resourceOut, DXGI_FORMAT resViewFmt)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	if (resViewFmt == DXGI_FORMAT_UNKNOWN)
		shaderResourceViewDesc.Format = textureDescription->Format;
	else
		shaderResourceViewDesc.Format = resViewFmt;

	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = textureDescription->MipLevels;

	HRESULT result = mpd3dDevice->CreateTexture2D(textureDescription, NULL, textureOut);
	if (FAILED(result))
		return false;

	result = mpd3dDevice->CreateShaderResourceView(*textureOut, &shaderResourceViewDesc, resourceOut);
	if (FAILED(result))
		return false;

	return true;
}

//RenderTarget* D3DRenderer::CreateRenderTarget(int width, int height, bool useDepthBuffer)
//{
//	RenderTarget* newRenderTarget = NE_NEW RenderTarget();
//	newRenderTarget->mpRenderTargetTexture = createTexture(Texture_RenderTarget | Texture_RGBA, width, height);
//
//	if (useDepthBuffer)
//		newRenderTarget->mpDepthTexture = createTexture(Texture_RenderTarget | Texture_Depth, width, height);
//
//	newRenderTarget->mWidth = width;
//	newRenderTarget->mHeight = height;
//
//	HRESULT result;
//
//	D3D11_RENDER_TARGET_VIEW_DESC renderViewDesc;
//
//	renderViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	renderViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
//	renderViewDesc.Texture2D.MipSlice = 0;
//
//	result = mpd3dDevice->CreateRenderTargetView(newRenderTarget->mpRenderTargetTexture->mpTexture, &renderViewDesc, &newRenderTarget->mpRenderTargetView);
//	if (FAILED(result))
//	{
//		NE_DELETE(newRenderTarget);
//		return NULL;
//	}
//
//	if (useDepthBuffer)
//	{
//		D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;
//		depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
//		depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//		depthViewDesc.Texture2D.MipSlice = 0;
//
//		result = mpd3dDevice->CreateDepthStencilView(newRenderTarget->mpDepthTexture->mpTexture, NULL, &newRenderTarget->mpDepthView);
//		if (FAILED(result))
//		{
//			NE_DELETE(newRenderTarget);
//			return NULL;
//		}
//	}
//
//	return newRenderTarget;
//}

//void D3DRenderer::setRenderTarget(RenderTarget* target)
//{
//	mpd3dImmediateContext->OMSetRenderTargets(1, &target->mpRenderTargetView, target->mpDepthView);
//}

void D3DRenderer::setBlendState(bool blendingEnabled)
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;

	if (blendingEnabled)
		context()->OMSetBlendState(mpBlendStateAlpha, blendFactor, sampleMask);
	else
		context()->OMSetBlendState(mpBlendStateOpaque, blendFactor, sampleMask);
}

void D3DRenderer::setDepthStencilState(DepthStencilState::Type state)
{
	mpd3dImmediateContext->OMSetDepthStencilState(mpDepthStencilStates[state], 0);
}

void D3DRenderer::setViewport(int x, int y, int width, int height)
{
	mScreenViewport.TopLeftX = (float)x;
	mScreenViewport.TopLeftY = (float)y;
	mScreenViewport.Width = (float)width;
	mScreenViewport.Height = (float)height;
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mpd3dImmediateContext->RSSetViewports(1, &mScreenViewport);
}

void D3DRenderer::setShader(Shader* shader)
{
	mpd3dImmediateContext->IASetInputLayout(shader->mpInputLayout);
	mpd3dImmediateContext->IASetPrimitiveTopology(shader->mPrimitiveTopology);

	//Set any active shaders and disable ones not in use
	mpd3dImmediateContext->VSSetShader(shader->mpVertexShader, NULL, 0);
	mpd3dImmediateContext->PSSetShader(shader->mpPixelShader, NULL, 0);
	mpd3dImmediateContext->GSSetShader(shader->mpGeometryShader, NULL, 0);
	mpd3dImmediateContext->CSSetShader(shader->mpComputeShader, NULL, 0);
	mpd3dImmediateContext->HSSetShader(shader->mpHullShader, NULL, 0);
	mpd3dImmediateContext->DSSetShader(shader->mpDomainShader, NULL, 0);

	mpActiveShader = shader;
}

void D3DRenderer::setShader(const std::string& name)
{
	auto it = mLoadedShaders.find(name);

	if (it != mLoadedShaders.end())
		setShader(it->second);
	else
		throw - 1;
}

novus::Shader* D3DRenderer::getShader(const std::string& name)
{
	auto it = mLoadedShaders.find(name);

	if (it != mLoadedShaders.end())
	{
		return it->second;
	}

	return nullptr;
}

void D3DRenderer::UnbindShader()
{
	mpActiveShader = NULL;

	//Set any active shaders and disable ones not in use
	mpd3dImmediateContext->VSSetShader(NULL, NULL, 0);
	mpd3dImmediateContext->PSSetShader(NULL, NULL, 0);
	mpd3dImmediateContext->GSSetShader(NULL, NULL, 0);
	mpd3dImmediateContext->CSSetShader(NULL, NULL, 0);
	mpd3dImmediateContext->HSSetShader(NULL, NULL, 0);
	mpd3dImmediateContext->DSSetShader(NULL, NULL, 0);
}

void D3DRenderer::ResetSamplerState()
{
	ID3D11SamplerState** samplerArr = NE_NEW ID3D11SamplerState*[8];

	for (int i = 0; i < 8; i++) samplerArr[i] = mpSamplerState;

	if (mpActiveShader)
	{
		if (mpActiveShader->hasVertexShader())
			mpd3dImmediateContext->VSSetSamplers(0, 8, samplerArr);
		if (mpActiveShader->hasPixelShader())
			mpd3dImmediateContext->PSSetSamplers(0, 8, samplerArr);
		if (mpActiveShader->hasGeometryShader())
			mpd3dImmediateContext->GSSetSamplers(0, 8, samplerArr);
		if (mpActiveShader->hasComputeShader())
			mpd3dImmediateContext->CSSetSamplers(0, 8, samplerArr);
		if (mpActiveShader->hasHullShader())
			mpd3dImmediateContext->HSSetSamplers(0, 8, samplerArr);
		if (mpActiveShader->hasDomainShader())
			mpd3dImmediateContext->DSSetSamplers(0, 8, samplerArr);
	}

	NE_DELETEARR(samplerArr);
}

void D3DRenderer::ResetRenderTarget()
{
	mpd3dImmediateContext->OMSetRenderTargets(1, &mpRenderTarget, mpDepthStencilView);
}
//
//void D3DRenderer::Clear(RenderTarget* target)
//{
//	mpd3dImmediateContext->ClearRenderTargetView(target->mpRenderTargetView, mClearColor);
//	mpd3dImmediateContext->ClearDepthStencilView(target->mpDepthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//}

void D3DRenderer::PreRender()
{
	if (!isUsingHMD())
	{
		mpd3dImmediateContext->ClearRenderTargetView(mpRenderTarget, mClearColor);
		mpd3dImmediateContext->ClearDepthStencilView(mpDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
	mpd3dImmediateContext->VSSetSamplers(0, 1, &mpSamplerState);
	mpd3dImmediateContext->PSSetSamplers(0, 1, &mpSamplerState);
	mpd3dImmediateContext->GSSetSamplers(0, 1, &mpSamplerState);
	mpd3dImmediateContext->DSSetSamplers(0, 1, &mpSamplerState);
	mpd3dImmediateContext->HSSetSamplers(0, 1, &mpSamplerState);

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;

	setBlendState(false);

	/*if (isUsingHMD())
	{
	mpOVRManager->UpdateTrackingState();
	mpOVRManager->BeginFrame();
	}
	*/
	//mGBuffer->clearRenderTargets();
	//mGBuffer->bindRenderTargets();
}

void D3DRenderer::PostRender()
{
	mpd3dImmediateContext->OMSetRenderTargets(1, &mpRenderTarget, mpDepthStencilView);

	//if (isUsingHMD())
	//	mpOVRManager->PostRender();
	//else
	mpSwapChain->Present(0, 0);
}

void D3DRenderer::RenderDeferredLighting()
{
	mpd3dImmediateContext->OMSetRenderTargets(1, &mpRenderTarget, mpDepthStencilView);

	//mpDeferredRenderer->Render(this);
}

void D3DRenderer::PushTransform(const Transform& transform)
{
	mMatrixStack.Push(transform.GetTransform());
}

void D3DRenderer::PopTransform()
{
	mMatrixStack.Pop();
}

Matrix4 D3DRenderer::GetTopTransform() const
{
	return mMatrixStack.getTop();
}

Matrix4 D3DRenderer::GetTopTransformInverse() const
{
	return mMatrixStack.getTopInverse();
}

bool D3DRenderer::isUsingHMD() const
{
	//return mUseHMD && mpOVRManager != NULL && mpOVRManager->IsDeviceConnected();

	return false;
}

}//namespace novus