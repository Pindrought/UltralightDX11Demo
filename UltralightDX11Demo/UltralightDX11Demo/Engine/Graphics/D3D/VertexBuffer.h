#pragma once
#include "Vertex.h"

template<class T>
class VertexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT stride = sizeof(T);
	std::vector<T> storage;
public:
	VertexBuffer() {}
	ID3D11Buffer* Get() const;
	ID3D11Buffer* const* GetAddressOf()const;
	UINT VertexCount() const;
	UINT Stride() const;
	const UINT* StridePtr() const;
	HRESULT Initialize(ID3D11Device* pDevice, std::vector<T> data);
};