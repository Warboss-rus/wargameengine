#pragma once

#include "Light.h"
#include "../../lib/src/Math/float4.h"

class COmniLight : public CLight
{
public:
	COmniLight(float3 const& position = float3(0, 0, 0));

	// Положение источника света
	void SetPosition(float3 const& position);
	// активизацтя источника света
	void SetLight(GLenum light)const;
	// параметры ослабления интенсивности в зависимости от расстояния
	void SetQuadraticAttenuation(GLfloat quadraticAttenuation);
	void SetLinearAttenuation(GLfloat linearAttenuation);
	void SetConstantAttenuation(GLfloat constantAttenuation);
	float3 GetPosition();
private:
	float3 m_position;
	GLfloat m_quadraticAttenuation;
	GLfloat m_linearAttenuation;
	GLfloat m_constantAttenuation;
};
