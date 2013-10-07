#include "ObjectInterface.h"
#include <string>

class C3DObject : public IObject
{
public:
	C3DObject(std::string model, double x, double y, double rotation);
	void Draw() const;
	void Move(double deltaX, double deltaY, double deltaRotate) { m_x += deltaX; m_y += deltaY; m_rotation += deltaRotate; }
	double GetX() const { return m_x; }
	double GetY() const { return m_y; }
	double GetZ() const {return 0; }
	double GetRotation() const { return m_rotation; }
	const double* GetBounding() const { return m_bounding; }
private:
	std::string m_model;
	double m_x;
	double m_y;
	double m_rotation;
	double m_bounding[6];
};