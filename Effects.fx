
Texture2D tetDiffuse;//:register(t0);
SamplerState sameLine;//:register(s0);
cbuffer cbPerObj
{
	float4x4 wvp;
};

struct VS_INPUT
{
	float4 pos:POSITION;
	float2 uv:TEXCOORD0;
	//float4 color:COLOR0;
	//float4 color2:COLOR1;
};
struct VS_OUTPUT
{
	float4 pos:SV_POSITION;
	float2 uv:TEXCOORD0;
	//float4 color:COLOR0;
	//float4 color2:COLOR1;
};

VS_OUTPUT VS(VS_INPUT input)//float4 col2 : COLOR1)
{
	VS_OUTPUT v_out;
	v_out.pos = mul(input.pos,wvp);//inPos;
	v_out.uv = input.uv;
	//v_out.color2 = col2;
	return v_out;
}

float4 PS(VS_OUTPUT input) :SV_TARGET
{
	float4 diffuseColor = tetDiffuse.Sample(sameLine,input.uv);
	//clip(diffuseColor.a - 0.25);
	return diffuseColor;
	//return saturate(input.color);// +input.color2);
}