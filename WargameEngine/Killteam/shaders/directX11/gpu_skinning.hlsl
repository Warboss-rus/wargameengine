struct sLightSource
{
	float3 pos;
	int enabled;
	float4 diffuse;
	float4 ambient;
	float4 specular;
};
struct sMaterial
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float4 SpecularColor;
	float Shininess;
};
#define NUMBEROFLIGHTS 1
#define NUMBEROFBONES 512
cbuffer Constant
{
	matrix joints[NUMBEROFBONES];
};
cbuffer Fragment
{
	float4 Color;
	sMaterial Material;
	sLightSource Lights[NUMBEROFLIGHTS];
};
cbuffer Vertex
{
	matrix mvp_matrix : WORLDVIEWPROJECTION;
};
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};
Texture2D shaderTexture : register(t0);
Texture2D shadowTexture : register(t1);
SamplerState SampleType;

PixelInputType VShader(float3 Pos : POSITION, float2 texCoords : TEXCOORD, float3 normal : NORMAL, int4 weightIndices : weightIndices, float4 weights : weights)
{
	PixelInputType result;
	float4 finalPos = float4(Pos, 1.0f);
	if(weights[0] > 0.0f)
	{
		finalPos = float4(0.0f, 0.0f, 0.0f, 0.0f);
		for(int i = 0; i < 4; ++i)
		{
			int index = weightIndices[i];
			finalPos += mul(float4(Pos, 1.0f), joints[index]) * weights[i];
		}
	}
	result.position = mul(finalPos, mvp_matrix);
	result.tex = texCoords;
	return result;
}
float4 PShader(PixelInputType input) : SV_TARGET
{
	float4 tex = shaderTexture.Sample(SampleType, input.tex);
	return tex + Color;
}