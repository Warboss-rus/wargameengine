class CCamera
{
public:
	static const double SCALE;
	static const double TRANSLATE;

	CCamera(void):m_maxTransX(8.0), m_maxTransY(8.0), m_maxScale(2.8), m_minScale(0.35) { Reset(); }
	void Update();
	void Translate(double transX, double transY);
	void Rotate(double rotZ, double rotX);
	void ZoomIn();
	void ZoomOut();
	void Reset();
	void SetLimits(double maxTransX, double maxTransY, double maxScale, double minScale);
	double GetTranslationX() { return m_transX; }
	double GetTranslationY() { return m_transY; }
	double GetScale() { return m_scale; }
private:
	double m_maxTransX;
	double m_maxTransY;
	double m_maxScale;
	double m_minScale;
	double m_transX;
	double m_transY;
	double m_rotX;
	double m_rotZ;
	double m_scale;
};