class CCamera
{
public:
	static const double SCALE;
	static const double TRANSLATE;

	CCamera(void):m_maxTransX(8.0), m_maxTransY(8.0), m_maxScale(1.9), m_minScale(0.35) { Reset(); }
	void Update();
	void Translate(double transX, double transY);
	void Rotate(double rotZ, double rotX);
	void ZoomIn();
	void ZoomOut();
	void Reset();
private:
	const double m_maxTransX;
	const double m_maxTransY;
	const double m_maxScale;
	const double m_minScale;
	double m_transX;
	double m_transY;
	double m_rotX;
	double m_rotZ;
	double m_scale;
};