#include "ConstantBuffer.h"

template<class T>
ID3D11Buffer* ConstantBuffer<T>::Get() const
{
	return buffer.Get();
}

template<class T>
ID3D11Buffer* const* ConstantBuffer<T>::GetAddressOf() const
{
	return buffer.GetAddressOf();
}

template<class T>
HRESULT ConstantBuffer<T>::Initialize(ID3D11Device* pDevice_, ID3D11DeviceContext* pDeviceContext_)
{
	pDeviceContext = pDeviceContext_;

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16))); //Align to 16 bytes
	desc.StructureByteStride = 0;

	HRESULT hr = pDevice_->CreateBuffer(&desc, 0, &buffer);
	return hr;
}

template<class T>
HRESULT ConstantBuffer<T>::ApplyChanges()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr))
	{
		return hr;
	}
	CopyMemory(mappedResource.pData, &data, sizeof(T));
	pDeviceContext->Unmap(buffer.Get(), 0);
	return hr;
}

template ConstantBuffer<DirectX::XMMATRIX>;
template ConstantBuffer<ConstantBufferType_Ultralight>;