#pragma once
#include <DirectXMath.h>

struct ConstantBufferType_Ultralight
{
    DirectX::XMFLOAT4 State;
    DirectX::XMMATRIX Transform;
    DirectX::XMFLOAT4 Scalar4[2];
    DirectX::XMFLOAT4 Vector[8];
    uint32_t ClipSize;
    DirectX::XMMATRIX Clip[8];
};