class CRuler
{
public:
	static void SetBegin(double x, double y);
	static void SetEnd(double x, double y);
	static void Draw();
	static void Hide();
	static double GetDistance();
private:
	static bool m_isVisible;
	static double m_worldBeginX;
	static double m_worldBeginY;
	static double m_worldEndX;
	static double m_worldEndY;
};