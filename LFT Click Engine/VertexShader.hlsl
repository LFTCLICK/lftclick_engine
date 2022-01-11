cbuffer cbuff
{
	matrix transform;
	float xOffset;
	float yOffset;
	float xFlip;
	float alphaOverride;
};
struct VSOut
{
	float2 tex : TEXCOORD;
	float4 pos : SV_Position;
	float alphaOverride : FOG;
	//float3 norm : NORMAL;
};
VSOut main(float3 pos : Position, /*float3 normal : NORMAL,*/ float2 tex : TEXCOORD)
{
	VSOut vso;
	vso.pos = mul(float4(pos, 1.0f), transform);
	//vso.norm = normal;
	vso.tex = float2(xFlip * (tex.x + xOffset), tex.y + yOffset);
	vso.alphaOverride = alphaOverride;
	return vso;
}