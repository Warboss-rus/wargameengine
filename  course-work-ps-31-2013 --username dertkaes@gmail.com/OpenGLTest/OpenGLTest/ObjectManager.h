#include "ObjectInterface.h"
#include <vector>
#include <string>

class CObjectManager
{
public:
	void Draw();
	void AddObject(IObject * object);
	void DeleteObject(IObject * object);
	IObject * GetObjectByCoords() { return NULL; }
private:
	std::vector<IObject *> m_objects;
};