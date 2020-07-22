#include "PixelShader.h"

HRESULT PixelShader::Initialize(ID3D11Device* pDevice_, std::wstring shaderpath_)
{
	std::wstring filePath = DirectoryHelper::GetExecutableDirectory() + shaderpath_;

	HRESULT hr = D3DReadFileToBlob(filePath.c_str(), &shader_buffer);

	if (FAILED(hr))
	{
		return hr;
	}

	hr = pDevice_->CreatePixelShader(shader_buffer.Get()->GetBufferPointer(), shader_buffer.Get()->GetBufferSize(), NULL, &shader);
	if (FAILED(hr))
	{
		return hr;
	}

	return hr;
}

ID3D11PixelShader* PixelShader::GetShader()
{
	return shader.Get();
}

ID3D10Blob* PixelShader::GetBuffer()
{
	return shader_buffer.Get();
}