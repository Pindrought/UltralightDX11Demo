#pragma once
#include "../../../pch.h"
#include "FileSystems/FileSystemDefault.h"
#include "GPUAdapters/GPUDriverD3D11.h"
#include "../D3D/VertexBuffer.h"

#include "LoadListeners/HtmlViewLoadListener.h"
#include "ViewListeners/HtmlViewViewListener.h"

class HtmlView
{
public:
	HtmlView(GPUDriverD3D11* pGpuDriver, ul::RefPtr<ul::View> view, VertexBuffer<Vertex_3pf_2tf>& vertexBuffer, uint32_t width, uint32_t height, bool isTransparent = true);
	void LoadURL(std::string url);
	bool IsLoading();
	void FireMouseEvent(ul::MouseEvent mouseEvent);
	void FireKeyboardEvent(ul::KeyEvent keyboardEvent);
	void Focus();
	void SetSize(uint32_t width, uint32_t height);
	void SetPosition(float x, float y);
	DirectX::XMMATRIX GetWorldMatrix();
	ID3D11ShaderResourceView* const* GetAddressOfShaderResourceView();
	VertexBuffer<Vertex_3pf_2tf>* GetVertexBuffer();
	~HtmlView();
private:
	void UpdateWorldMatrix();
	void RegisterNativeCFunctions();
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
	VertexBuffer<Vertex_3pf_2tf>& vertexBuffer;
	GPUDriverD3D11* pGpuDriver = nullptr;
	ul::RefPtr<ul::View> view;
	std::unique_ptr<HtmlViewLoadListener> viewLoadListener;
	std::unique_ptr<HtmlViewViewListener> viewViewListener;

	uint32_t width = 0;
	uint32_t height = 0;
	DirectX::XMFLOAT2 position = { 0, 0 };
	bool isTransparent = false;
};