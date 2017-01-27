#pragma once
#include <string.h>
#include "../view/Vector3.h"

class CLight
{
public:
	CLight(CVector3f const& position, float distance = 0.0f)
		: m_position(position)
		, m_disatance(distance)
	{}

	void SetPosition(CVector3f const& position) { m_position = position; }
	void SetAmbientColor(float * ambientColor) { memcpy(m_ambient, ambientColor, sizeof(float) * 4); }
	void SetDiffuseColor(float * diffuseColor) { memcpy(m_diffuse, diffuseColor, sizeof(float) * 4); }
	void SetSpecularColor(float * specularColor) { memcpy(m_specular, specularColor, sizeof(float) * 4); }

	const CVector3f& GetPosition() const { return m_position; }
	const float* GetAmbient() const { return m_ambient; }
	const float* GetDiffuse() const { return m_diffuse; }
	const float* GetSpecular() const { return m_specular; }
	float GetDistance() const { return m_disatance; }
private:
	CVector3f m_position;
	float m_disatance;
	float m_ambient[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	float m_diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float m_specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
};