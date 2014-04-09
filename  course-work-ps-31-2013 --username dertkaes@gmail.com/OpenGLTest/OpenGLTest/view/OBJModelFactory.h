#include <string>
#include "3dModel.h"

void * LoadObjModel(void* data, unsigned int size, void* param);

struct sOBJLoader
{
	std::string path;
	C3DModel * oldmodel;
	C3DModel * newModel;
};