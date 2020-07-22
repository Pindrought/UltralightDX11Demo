#include "VertexShader.h"

HRESULT VertexShader::Initialize(ID3D11Device* pDevice_, std::wstring shaderpath_, std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc_)
{
	std::wstring filePath = DirectoryHelper::GetExecutableDirectory() + shaderpath_;

	HRESULT hr = D3DReadFileToBlob(filePath.c_str(), shader_buffer.GetAddressOf());
	if (FAILED(hr))
		return hr;

	hr = pDevice_->CreateVertexShader(shader_buffer->GetBufferPointer(), shader_buffer->GetBufferSize(), NULL, &shader);
	if (FAILED(hr))
		return hr;

	hr = pDevice_->CreateInputLayout(layoutDesc_.data(), layoutDesc_.size(), shader_buffer->GetBufferPointer(), shader_buffer->GetBufferSize(), &inputLayout);
	if (FAILED(hr))
		return hr;

	return hr;
}

ID3D11VertexShader* VertexShader::GetShader()
{
	return shader.Get();
}

ID3D10Blob* VertexShader::GetBuffer()
{
	return shader_buffer.Get();
}

ID3D11InputLayout* VertexShader::GetInputLayout()
{
	return inputLayout.Get();
}