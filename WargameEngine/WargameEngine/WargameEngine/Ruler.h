#pragma once
#include "view/Vector3.h"

class CRuler
{
public:
	CRuler();
	void Enable();
	void Disable();
	void SetBegin(double x, double y);
	void SetEnd(double x, double y);
	void Hide();
	double GetDistance() const;
	bool IsVisible() const;
	bool IsEnabled() const;
	CVector3d GetBegin() const;
	CVector3d GetEnd() const;
private:
	double m_worldBeginX;
	double m_worldBeginY;
	double m_worldEndX;
	double m_worldEndY;
	bool m_isVisible;
	bool m_enabled;
};