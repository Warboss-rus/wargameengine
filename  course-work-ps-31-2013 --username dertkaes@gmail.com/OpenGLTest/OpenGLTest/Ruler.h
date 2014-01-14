class CRuler
{
public:
	CRuler();
	void SetBegin(float x, float y);
	void SetEnd(float x, float y);
	void Draw() const;
	float GetDistance() const;
	void Hide();
	void PrintText( float x, float y, const char *st) const;
private:
	bool m_isVisible;
	float m_worldBeginX;
	float m_worldBeginY;
	float m_worldEndX;
	float m_worldEndY;
};