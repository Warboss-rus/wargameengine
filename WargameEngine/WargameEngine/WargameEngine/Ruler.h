#pragma once

class CRuler
{
public:
	CRuler();
	void Enable();
	void Disable();
	void SetBegin(double x, double y);
	void SetEnd(double x, double y);
	void Draw() const;
	void Hide();
	double GetDistance() const;
	bool IsVisible() const;
	bool IsEnabled() const;
private:
	bool m_isVisible;
	double m_worldBeginX;
	double m_worldBeginY;
	double m_worldEndX;
	double m_worldEndY;
	bool m_enabled;
};