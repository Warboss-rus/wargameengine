class CCamera
{
public:
	CCamera(void):m_maxTransX(10.0), m_maxTransY(10.0), m_maxScale(10.0), m_minScale(0.31) { Reset(); }
	void Update();
	void Translate(double transX, double transY);
	void Rotate(double rotZ, double rotX);
	void Scale(double scale);
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