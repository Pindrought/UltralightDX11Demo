#pragma once
#include "../../../../pch.h"

class VertexShader
{
public:
	HRESULT Initialize(ID3D11Device* pDevice, std::wstring shaderpath, std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc);
	ID3D11VertexShader* GetShader();
	ID3D10Blob* GetBuffer();
	ID3D11InputLayout* GetInputLayout();
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};