#include "ObjectInterface.h"
#include<vector>

class C3DObject : public IObject
{
public:
	C3DObject();
	void Draw();
private:
	std::vector<double> a;
	unsigned int m_count;
};