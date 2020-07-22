#pragma once
#include "../../../pch.h"

class Window;
class D3DClass
{
public:
	bool Initialize(Window* window);
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

private:
	bool InitializeDeviceAndSwapchain();
	bool InitializeRenderTarget();
	bool InitializeDepthStencilBufferAndState();
	bool InitializeRasterizerState();
	bool InitializeSamplerState();

	HWND hwnd = NULL;
	uint16_t width = 0;
	uint16_t height = 0;
};