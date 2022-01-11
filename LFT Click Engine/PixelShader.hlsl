Texture2D tex;
SamplerState splr;


struct VSOut
{
	float2 tc : TEXCOORD;
	float4 pos : SV_Position;
	//float3 norm : NORMAL;
};
float4 main(float2 tc : TEXCOORD, float alphaOverride : FOG) : SV_Target
{
	float4 textureColor = tex.Sample(splr,tc);
	clip(textureColor.a < 0.1f ? -1 : 1);
	if (textureColor.a > 0 && alphaOverride < .99)
		textureColor.a *= alphaOverride;
	return textureColor;
//return float4(vs.norm,1.0);
}