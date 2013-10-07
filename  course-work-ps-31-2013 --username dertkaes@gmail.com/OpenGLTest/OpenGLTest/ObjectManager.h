#include "ObjectInterface.h"
#include <vector>
#include <string>

class CObjectManager
{
public:
	void Draw();
	void AddObject(IObject * object);
	void DeleteObject(IObject * object);
	IObject * GetNearestObjectByVector(double startx, double starty, double startz, double endx, double endy, double endz) const;
private:
	std::vector<IObject *> m_objects;
};