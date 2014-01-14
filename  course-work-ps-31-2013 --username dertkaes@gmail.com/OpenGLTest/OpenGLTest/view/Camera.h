class CCamera
{
public:
	static const float SCALE;
	static const float TRANSLATE;

	CCamera(void):m_maxTransX(8.0), m_maxTransY(8.0), m_maxScale(2.8), m_minScale(0.35) { Reset(); }
	void Update();
	void Translate(float transX, float transY);
	void Rotate(float rotZ, float rotX);
	void ZoomIn();
	void ZoomOut();
	void Reset();
	void SetLimits(float maxTransX, float maxTransY, float maxScale, float minScale);
	float GetTranslationX() { return m_transX; }
	float GetTranslationY() { return m_transY; }
	float GetScale() { return m_scale; }
private:
	float m_maxTransX;
	float m_maxTransY;
	float m_maxScale;
	float m_minScale;
	float m_transX;
	float m_transY;
	float m_rotX;
	float m_rotZ;
	float m_scale;
};