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
cbuffer Constant
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

PixelInputType VShader(float3 Pos : POSITION, float2 texCoords : TEXCOORD, float3 normal : NORMAL, float4 instancePosition : instancePosition, float2 instanceTexCoordPos : instanceTexCoordPos)
{
	PixelInputType result;
	result.position = mul(float4(Pos * instancePosition.w + instancePosition.xyz, 1.0f), mvp_matrix);
	result.tex = texCoords + instanceTexCoordPos;
	return result;
}
float4 PShader(PixelInputType input) : SV_TARGET
{
	float4 tex = shaderTexture.Sample(SampleType, input.tex);
	return tex + Color;
}