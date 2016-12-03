#pragma once
#include "view/Vector3.h"

class CRuler
{
public:
	CRuler();
	void Enable();
	void Disable();
	void SetBegin(float x, float y);
	void SetEnd(float x, float y);
	void Hide();
	double GetDistance() const;
	bool IsVisible() const;
	bool IsEnabled() const;
	CVector3f GetBegin() const;
	CVector3f GetEnd() const;
private:
	bool m_enabled;
	bool m_isVisible;
	float m_worldBeginX;
	float m_worldEndX;
	float m_worldBeginY;
	float m_worldEndY;
};