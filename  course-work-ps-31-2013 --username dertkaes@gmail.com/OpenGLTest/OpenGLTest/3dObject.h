#include "ObjectInterface.h"
#include "3dModel.h"

class C3DObject : public IObject
{
public:
	C3DObject(C3DModel * model):m_model(model), m_x(0), m_y(0), m_rotation(0) {}
	void Draw() {m_model->Draw(); }
private:
	C3DModel * m_model;
	int m_x;
	int m_y;
	int m_rotation;
};