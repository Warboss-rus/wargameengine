struct sLightSource
{
	float3 pos;
	float4 diffuse;
	float4 ambient;
	float4 specular;
	bool enabled;
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
	matrix model_matrix : WORLDVIEW;
};
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPosition : POSITION;
};
Texture2D shaderTexture : register(t0);
Texture2D shadowTexture : register(t1);
SamplerState SampleType;

PixelInputType VShader(float3 Pos : POSITION, float2 texCoords : TEXCOORD, float3 normal : NORMAL)
{
	PixelInputType result;
	result.position = mul(float4(Pos, 1.0f), mvp_matrix);
	result.tex = texCoords;
	result.normal = mul(float4(normal, 1.0f), model_matrix).xyz;
	result.worldPosition = mul(float4(Pos, 1.0f), model_matrix).xyz;
	return result;
}

float4 CalculateDiffuseColor(
	float3 normal,	// must be normalized 
	float3 light,		// must be normalized
	float4 diffuseLight,
	float4 diffuseMaterial,
	float attenuation
	)
{
	float diffuseFactor = max(dot(normal, light), 0.0) * attenuation;
	
	return diffuseMaterial * diffuseFactor * diffuseLight;
}

float4 CalculateSpecularColor(
	float3 reflectedLight,
	float3 eye,
	float4 specularLight,
	float4 specularMaterial,
	float shininess,
	float attenuation
	)
{
	float specularFactor = max(dot(reflectedLight, eye), 0.0f);	
	float specularIntensity = pow(specularFactor, shininess) * attenuation;
	if (dot(reflectedLight, eye) < 0)
        {
             return float4(0.4f, 0.4f, 0.4f, 1.0f);
        }
	return specularLight * specularIntensity * specularMaterial;
}

float4 PShader(PixelInputType input) : SV_TARGET
{
	float4 color = shaderTexture.Sample(SampleType, input.tex);
	
	float3 eyeDir = -input.worldPosition;
	float3 lightDir = normalize(Lights[0].pos - eyeDir);
	
	float4 ambientComponent = Lights[0].ambient * Material.AmbientColor;
	
	float4 diffuseComponent = CalculateDiffuseColor(normalize(input.normal), lightDir, Lights[0].diffuse, Material.DiffuseColor, 1.0f);
	
	float3 reflectedLight = reflect(-lightDir, normalize(input.normal));
	float4 specularComponent = CalculateSpecularColor(reflectedLight, normalize(eyeDir), Lights[0].specular, Material.SpecularColor, Material.Shininess, 1.0f);
	
	float4 result = color * saturate(ambientComponent + diffuseComponent + saturate(specularComponent));
	
	return result;
}