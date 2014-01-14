#pragma once
#include "../gl.h"
#include "../../lib/src/Math/float4.h"

class CLight
{
public:
	virtual ~CLight(void);
	virtual void SetLight(GLenum light)const;
	float4 GetColor()
	{
		return float4(m_diffuse[0], m_diffuse[1], m_diffuse[2], m_diffuse[3]);
	};
	void SetDiffuseIntensity(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1);
	void SetAmbientIntensity(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1);
	void SetSpecularIntensity(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1);
protected:
	CLight(void);
private:
	GLfloat m_diffuse[4];
	GLfloat m_ambient[4];
	GLfloat m_specular[4];
};
