#pragma once
#include "../../../pch.h"
#include "ConstantBufferTypes.h"

template<class T>
class ConstantBuffer
{
public:
	ConstantBuffer() {}
	T data;
	ID3D11Buffer* Get()const;
	ID3D11Buffer* const* GetAddressOf()const;
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT ApplyChanges();
private:
	ID3D11DeviceContext* pDeviceContext = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};