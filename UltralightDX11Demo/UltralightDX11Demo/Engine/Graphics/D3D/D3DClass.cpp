#include "D3DClass.h"
#include "../../Window/Window.h"

bool D3DClass::Initialize(Window* window_)
{
	hwnd = window_->GetHWND();
	width = window_->GetWidth();
	height = window_->GetHeight();

	if (!InitializeDeviceAndSwapchain())
		return false;

	if (!InitializeRenderTarget())
		return false;

	if (!InitializeDepthStencilBufferAndState())
		return false;

	if (!InitializeRasterizerState())
		return false;

	if (!InitializeSamplerState())
		return false;

	return true;
}

bool D3DClass::InitializeDeviceAndSwapchain()
{
	DXGI_SWAP_CHAIN_DESC scd = { 0 };

	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1; //double buffered by default in windowed mode
	scd.OutputWindow = hwnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(nullptr, //IDXGI Adapter
									D3D_DRIVER_TYPE_HARDWARE,
									0, //FOR SOFTWARE DRIVER TYPE
									D3D11_CREATE_DEVICE_DEBUG, //FLAGS FOR RUNTIME LAYERS
									nullptr, //FEATURE LEVELS ARRAY
									0, //# OF FEATURE LEVELS IN ARRAY
									D3D11_SDK_VERSION,
									&scd, //Swapchain description
									&swapchain, //Swapchain Address
									&device, //Device Address
									NULL, //Supported feature level
									&deviceContext); //Device Context Address

	ReturnFalseIfFail(hr, "D3D11 Device/DeviceContext/Swapchain creation failed.");
	return true;
}

bool D3DClass::InitializeRenderTarget()
{
	//Get backbuffer from swapchain so that we can create primary render target view for window
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	ReturnFalseIfFail(hr, "D3D11 Swapchain backbuffer retrieval failed.");

	//Create primary render target view for window
	hr = device->CreateRenderTargetView(backBuffer.Get(), NULL, &renderTargetView);
	ReturnFalseIfFail(hr, "D3D11 Failed to create render target view.");

	return true;
}

bool D3DClass::InitializeDepthStencilBufferAndState()
{
	//Describe our Depth/Stencil Buffer
	CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, width, height); //24 bits for depth, 8 bits for stencil
	depthStencilTextureDesc.MipLevels = 1;
	depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilTextureDesc.SampleDesc.Count = 1;
	depthStencilTextureDesc.SampleDesc.Quality = 0;

	HRESULT hr = device->CreateTexture2D(&depthStencilTextureDesc, NULL, &depthStencilBuffer);
	ReturnFalseIfFail(hr, "D3D11 Failed to create texture for depth stencil buffer.");

	hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, &depthStencilView);
	ReturnFalseIfFail(hr, "D3D11 Failed to create depth stencil view.");

	//Initialize depth stencil state
	CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL; //less than or equals standard depth function comparison

	hr = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	ReturnFalseIfFail(hr, "D3D11 Failed to create depth stencil state.");

	return true;
}

bool D3DClass::InitializeRasterizerState()
{
	CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	HRESULT hr = device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	ReturnFalseIfFail(hr, "D3D11 Failed to create rasterizer state.");
	return true;
}

bool D3DClass::InitializeSamplerState()
{
	CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	HRESULT hr = device->CreateSamplerState(&sampDesc, &samplerState); //Create sampler state
	ReturnFalseIfFail(hr, "D3D11 Failed to initialize sampler state.");
	return true;
}
