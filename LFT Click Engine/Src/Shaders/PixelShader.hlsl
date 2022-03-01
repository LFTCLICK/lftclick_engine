Texture2D diffuse;
SamplerState samState;

struct VSOut
{
	float2 tex : TEXCOORD;
	float4 pos : SV_Position;
	float alphaOverride : FOG;
};

float4 main(VSOut pin) : SV_Target
{
	float4 textureColor = diffuse.Sample(samState, pin.tex);

	clip(textureColor.a < 0.1f ? -1 : 1);

	if (textureColor.a > 0 && pin.alphaOverride < .99)
		textureColor.a *= pin.alphaOverride;

	return textureColor;
}