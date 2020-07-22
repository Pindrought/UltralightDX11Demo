#pragma pack_matrix( row_major )

cbuffer perSceneBuffer : register(b0)
{
	float4x4 orthoMatrix;
}; 

cbuffer perObjectBuffer : register(b1)
{
	float4x4 worldMatrix;
}

struct VS_INPUT
{
	float3 inPos : POSITION;
	float2 inTexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 outPosition : SV_POSITION;
	float2 outTexCoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.outPosition = mul(float4(input.inPos, 1.0f), mul(worldMatrix, orthoMatrix));
	output.outTexCoord = input.inTexCoord;
	return output;
}