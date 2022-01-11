
float4 main(float2 tc : TEXCOORD) : SV_Target
{

	return float4(tc,0,1);
	//return float4(vs.norm,1.0);
}