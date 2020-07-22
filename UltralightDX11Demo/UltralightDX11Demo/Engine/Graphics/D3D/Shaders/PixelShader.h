#pragma once
#include "../../../../pch.h"

class PixelShader
{
public:
	HRESULT Initialize(ID3D11Device* pDevice, std::wstring shaderpath);
	ID3D11PixelShader* GetShader();
	ID3D10Blob* GetBuffer();
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer;
};