#pragma once
#include "../../../../pch.h"

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

struct GeometryEntry
{
	ul::VertexBufferFormat format;
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
};

struct TextureEntry
{
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> textureSRV;
	bool isMSAARenderTarget = false;
	bool needsResolve = false;
	ComPtr<ID3D11Texture2D> resolveTexture;
	ComPtr<ID3D11ShaderResourceView> resolveSRV;
};

struct RenderTargetEntry
{
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	uint32_t renderTargetTextureId;
};