class CRuler
{
public:
	CRuler() { Hide(); }
	void SetBegin(double x, double y);
	void SetEnd(double x, double y);
	void Draw() const;
	double GetDistance() const;
	void Hide();
	void PrintText( double x, double y, const char *st) const;
private:
	bool m_isVisible;
	double m_worldBeginX;
	double m_worldBeginY;
	double m_worldEndX;
	double m_worldEndY;
};