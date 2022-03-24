Texture2D diffuse;
SamplerState samState;

struct VSOut
{
	float2 tex : TEXCOORD;
	float4 pos : SV_Position;
};

cbuffer cbPerObject
{
	float alphaOverride;
	float3 padding;
};

float4 main(VSOut pin) : SV_Target
{
	float4 textureColor = diffuse.Sample(samState, pin.tex);
	clip(textureColor.a - 0.1f);

	if (textureColor.a > 0 && alphaOverride < .99)
		textureColor.a *= alphaOverride;

	return textureColor;
}