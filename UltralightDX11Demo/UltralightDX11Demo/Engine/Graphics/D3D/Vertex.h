#pragma once
#include "../../../pch.h"

class Vertex_3pf_2tf
{
public:
	Vertex_3pf_2tf(float x_, float y_, float z_, float u_, float v_)
	{
		position = DirectX::XMFLOAT3(x_, y_, z_);
		texture = DirectX::XMFLOAT2(u_, v_);
	}
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture;
};