#pragma once
#include "../../pch.h"
#include "D3D/D3DClass.h"
#include "D3D/ConstantBuffer.h"
#include "D3D/Shaders/VertexShader.h"
#include "D3D/Shaders/PixelShader.h"
#include "D3D/VertexBuffer.h"
#include "UltralightImplementation/HtmlView.h"

class Window;
class Renderer
{
public:
	bool Initialize(Window* pWindow);
	void BeginFrame();
	void EndFrame();
	void RenderView(std::shared_ptr<HtmlView> view);
	D3DClass* GetD3DPtr();
private:
	bool InitializeShaders();
	bool InitializeConstantBuffers();
	D3DClass d3d;
	ConstantBuffer<DirectX::XMMATRIX> cb_orthoMatrix;
	ConstantBuffer<DirectX::XMMATRIX> cb_worldMatrix;
	PixelShader ps_orthographic2d;
	VertexShader vs_orthographic2d;
	Window* pWindow = nullptr;
};