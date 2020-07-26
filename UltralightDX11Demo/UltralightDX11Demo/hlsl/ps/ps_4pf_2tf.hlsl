#define GAMMA 2.1f

struct PS_INPUT
{
	float4 inPosition : SV_POSITION;
	float2 inTexCoord : TEXCOORD;
};

float3 ToSRGBSpace(float3 color)
{
	return pow(color, 1.0 / GAMMA);
}

float4 ToSRGBSpace(float4 color)
{
	return float4(ToSRGBSpace(color.rgb), color.a);
}

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 sampleColor = ToSRGBSpace(objTexture.Sample(objSamplerState, input.inTexCoord));
	return sampleColor;
}