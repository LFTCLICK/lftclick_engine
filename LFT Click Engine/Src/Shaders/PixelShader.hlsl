Texture2D diffuse : register(t0);
Texture2D darknessTex : register(t1);

SamplerState samState : register(s0);

struct VSOut
{
	float2 tex : TEXCOORD;
	float4 pos : SV_Position;
};

cbuffer cbPerObject
{
	float darknessFactor;
	float3 padding;
};

float4 main(VSOut pin) : SV_Target
{
	float4 textureColor = diffuse.Sample(samState, pin.tex);

	clip(textureColor.a - 0.1f);

	float4 darkness = darknessTex.Sample(samState, pin.tex);
	float4 nightColor = textureColor * darkness;

	float4 finalColor = lerp(textureColor, nightColor, darknessFactor);

	//textureColor *= darkness;


//	if (textureColor.a > 0 && alphaOverride < .99)
	//	textureColor.a = alphaOverride;

	return finalColor;
}