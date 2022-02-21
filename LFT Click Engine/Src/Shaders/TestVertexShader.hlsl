cbuffer cbuff
{
	matrix transform;
};
struct VSOut
{
	float2 tex : TEXCOORD;
	float4 pos : SV_Position;
	//float3 norm : NORMAL;
};
VSOut main(float3 pos : Position, /*float3 normal : NORMAL,*/ float2 tex : TEXCOORD)
{
	VSOut vso;
	vso.pos = mul(float4(pos, 1.0f), transform);
	//vso.norm = normal;
	vso.tex = tex;
	return vso;
}