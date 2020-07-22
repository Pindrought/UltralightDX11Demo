#include "VertexBuffer.h"

template<class T>
ID3D11Buffer* VertexBuffer<T>::Get() const
{
	return buffer.Get();
}

template<class T>
ID3D11Buffer* const* VertexBuffer<T>::GetAddressOf() const
{
	return buffer.GetAddressOf();
}

template<class T>
UINT VertexBuffer<T>::VertexCount() const
{
	return static_cast<UINT>(storage.size());
}

template<class T>
UINT VertexBuffer<T>::Stride() const
{
	return stride;
}

template<class T>
const UINT* VertexBuffer<T>::StridePtr() const
{
	return &stride;
}

template<class T>
HRESULT VertexBuffer<T>::Initialize(ID3D11Device* device_, std::vector<T> data_)
{
	storage = data_;

	if (data_.size() == 0) //if empty buffer 
	{
		return E_FAIL;
	}
	else
	{
		D3D11_BUFFER_DESC vertexBufferDesc = {};

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = stride * storage.size();
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data_.data();

		HRESULT hr = device_->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &buffer);
		return hr;
	}
}

template VertexBuffer<Vertex_3pf_2tf>;