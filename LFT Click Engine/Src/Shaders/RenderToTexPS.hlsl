Texture2D inputTex : register(t0);
Texture2D darknessTex1 : register(t1);
Texture2D darknessTex2 : register(t2);

SamplerState samState : register(s0);

struct VSOut
{
	float2 tex : TEXCOORD;
	float4 pos : SV_Position;
};

cbuffer cbPerObject
{
	float darknessFactor;
	float rednessFactor;
	float fadeFactor;
	float darknessSwitchFactor;
};

float4 main(VSOut pin) : SV_Target
{
	float4 textureColor = inputTex.Sample(samState, pin.tex);
	clip(textureColor.a - 0.1f);

	float4 darkness1 = darknessTex1.Sample(samState, pin.tex);
	float4 darkness2 = darknessTex2.Sample(samState, pin.tex);
	float4 darkness = lerp(darkness1, darkness2, darknessSwitchFactor);
	float4 redness = float4(darkness.r, 0, 0, 1);

	float4 nightColor = textureColor * darkness;
	float4 damagedColor = textureColor * redness;
	float4 blackColor = float4(0, 0, 0, 1);

	float4 daynightColor = lerp(textureColor, nightColor, darknessFactor);
	float4 outputColor = lerp(daynightColor, damagedColor, rednessFactor);
	float4 finalColor = lerp(outputColor, blackColor, fadeFactor);

	float gamma = 2.2f;
	finalColor.r = pow(finalColor.r, 1.0f / gamma);
	finalColor.g = pow(finalColor.g, 1.0f / gamma);
	finalColor.b = pow(finalColor.b, 1.0f / gamma);

	return finalColor;
}