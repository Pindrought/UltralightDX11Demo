#include "Renderer.h"
#include "../Window/Window.h"
#include "D3D/InputLayoutDescriptions.h"

bool Renderer::Initialize(Window* pWindow_)
{
	pWindow = pWindow_;
	if (!d3d.Initialize(pWindow))
		return false;

	if (!InitializeConstantBuffers())
		return false;

	if (!InitializeShaders())
		return false;

	return true;
}

void Renderer::BeginFrame()
{
	d3d.deviceContext->ClearState();

	d3d.deviceContext->OMSetRenderTargets(1, d3d.renderTargetView.GetAddressOf(), d3d.depthStencilView.Get());
	d3d.deviceContext->OMSetDepthStencilState(d3d.depthStencilState.Get(), 0);
	d3d.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(pWindow->GetWidth()), static_cast<float>(pWindow->GetHeight()));
	d3d.deviceContext->RSSetViewports(1, &viewport);
	d3d.deviceContext->RSSetState(d3d.rasterizerState.Get());
	d3d.deviceContext->PSSetSamplers(0, 1, d3d.samplerState.GetAddressOf());

	float backgroundColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	d3d.deviceContext->ClearRenderTargetView(d3d.renderTargetView.Get(), backgroundColor);
	d3d.deviceContext->ClearDepthStencilView(d3d.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}

void Renderer::EndFrame()
{
	d3d.swapchain->Present(1, NULL);
}

void Renderer::RenderView(std::shared_ptr<HtmlView> view_)
{
	if (view_->IsLoading() == false)
	{
		cb_worldMatrix.data = view_->GetWorldMatrix();
		cb_worldMatrix.ApplyChanges();

		d3d.deviceContext->IASetInputLayout(vs_orthographic2d.GetInputLayout());
		d3d.deviceContext->PSSetShader(ps_orthographic2d.GetShader(), nullptr, 0);
		d3d.deviceContext->VSSetShader(vs_orthographic2d.GetShader(), nullptr, 0);
		d3d.deviceContext->VSSetConstantBuffers(0, 1, cb_orthoMatrix.GetAddressOf());
		d3d.deviceContext->VSSetConstantBuffers(1, 1, cb_worldMatrix.GetAddressOf());
		d3d.deviceContext->PSSetShaderResources(0, 1, view_->GetAddressOfShaderResourceView());

		UINT offsets = 0;
		auto vb_view = view_->GetVertexBuffer();
		d3d.deviceContext->IASetVertexBuffers(0, 1, vb_view->GetAddressOf(), vb_view->StridePtr(), &offsets);
		d3d.deviceContext->Draw(vb_view->VertexCount(), 0);
	}
}

D3DClass* Renderer::GetD3DPtr()
{
	return &d3d;
}

bool Renderer::InitializeShaders()
{
	HRESULT hr = ps_orthographic2d.Initialize(d3d.device.Get(), L"ps_4pf_2tf.cso");
	ReturnFalseIfFail(hr, "Renderer failed to initialize orthographic 2d pixel shader.");

	hr = vs_orthographic2d.Initialize(d3d.device.Get(), L"vs_3pf_2tf.cso", InputLayoutDescription_3pf_2tf);
	ReturnFalseIfFail(hr, "Renderer failed to initialize orthographic 2d pixel shader.");

	return true;
}

bool Renderer::InitializeConstantBuffers()
{
	//Orthographic Matrix Constant Buffer
	HRESULT hr = cb_orthoMatrix.Initialize(d3d.device.Get(), d3d.deviceContext.Get());
	ReturnFalseIfFail(hr, "Renderer failed to initialize constant buffer for orthographic matrix.");
	cb_orthoMatrix.data = DirectX::XMMatrixOrthographicOffCenterLH(0, pWindow->GetWidth(), pWindow->GetHeight(), 0, 0, 100);
	hr = cb_orthoMatrix.ApplyChanges();
	ReturnFalseIfFail(hr, "Renderer failed to apply changes to constant buffer for orthographic matrix.");

	//Quad World Matrix Constant Buffer
	hr = cb_worldMatrix.Initialize(d3d.device.Get(), d3d.deviceContext.Get());
	ReturnFalseIfFail(hr, "Renderer failed to initialize constant buffer for quad world matrix.");
	cb_worldMatrix.data = DirectX::XMMatrixScaling(pWindow->GetWidth()/2, pWindow->GetHeight()/2, 1);
	hr = cb_worldMatrix.ApplyChanges();
	ReturnFalseIfFail(hr, "Renderer failed to apply changes to constant buffer for quad world matrix.");

	return true;
}
