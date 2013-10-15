class CRuler
{
public:
	CRuler() { Hide(); }
	void SetBegin(int windowBeginX, int windowBeginY);
	void SetEnd(int windowEndX, int windowEndY);
	void Draw() const;
	double GetDistance() const;
	void Hide();
	void PrintText( double x, double y, char *st) const;
private:
	bool m_isVisible;
	double m_worldBeginX;
	double m_worldBeginY;
	double m_worldEndX;
	double m_worldEndY;
};