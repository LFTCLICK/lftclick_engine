cbuffer cbPerObject
{
	row_major matrix transform;
	float2 Offset;
	float2 Scale;
	float xFlip;
	float darknessFactor;
	float2 padding;
};
struct VSOut
{
	float2 tex : TEXCOORD;
	float4 pos : SV_Position;
};

struct VSIn
{
	float3 PosWorld : Position;
	float2 TexCoord : TEXCOORD0;
};

VSOut main(VSIn vin)
{
	VSOut vout;
	vout.pos = mul(float4(vin.PosWorld, 1.0f), transform);
	
	vout.tex = float2(xFlip * (Scale.x * vin.TexCoord.x + Offset.x), (Scale.y * vin.TexCoord.y) + Offset.y);

	return vout;
}