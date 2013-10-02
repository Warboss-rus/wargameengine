#include "ObjectInterface.h"
#include "3dModel.h"

class C3DObject : public IObject
{
public:
	C3DObject(C3DModel * model, double x, double y, double rotation):m_model(model), m_x(x), m_y(y), m_rotation(rotation) {}
	void Draw();
	void Move(double deltaX, double deltaY, double deltaRotate) { m_x += deltaX; m_y += deltaY; m_rotation += deltaRotate; }
private:
	C3DModel * m_model;
	double m_x;
	double m_y;
	double m_rotation;
};