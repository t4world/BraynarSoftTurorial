
Texture2D tetDiffuse;//:register(t0);
SamplerState sameLine;//:register(s0);
cbuffer cbPerObj
{
	float4x4 wvp;
	float4x4 world;
};

struct Light
{
	float3 dir;
	float4 ambient;
	float4 diffuse;
};

cbuffer cbPerFrame
{
	Light light;
};

struct VS_INPUT
{
	float4 pos:POSITION;
	float2 uv:TEXCOORD0;
	float3 normal:NORMAL;
	//float4 color:COLOR0;
	//float4 color2:COLOR1;
};
struct VS_OUTPUT
{
	float4 pos:SV_POSITION;
	float2 uv:TEXCOORD0;
	float3 normal:NORMAL;
	//float4 color:COLOR0;
	//float4 color2:COLOR1;
};

VS_OUTPUT VS(VS_INPUT input)//float4 col2 : COLOR1)
{
	VS_OUTPUT v_out;
	v_out.pos = mul(input.pos,wvp);//inPos;
	v_out.uv = input.uv;
	v_out.normal = mul(input.normal, world);
	//v_out.color2 = col2;
	return v_out;
}

float4 PS(VS_OUTPUT input) :SV_TARGET
{
	float4 diffuseColor = tetDiffuse.Sample(sameLine,input.uv);
	//clip(diffuseColor.a - 0.25);
	float3 finalColor;
	finalColor = diffuseColor * light.ambient;
	finalColor += saturate(dot(light.dir, input.normal)) * light.diffuse * diffuseColor;
	return float4(finalColor, diffuseColor.a);
	//return saturate(input.color);// +input.color2);
}

float4 D2D_PS(VS_OUTPUT input) : SV_TARGET
{
	float4 diffuse = tetDiffuse.Sample(sameLine, input.uv);

	return diffuse;
}