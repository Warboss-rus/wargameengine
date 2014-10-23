#include <string>
#include "3dModel.h"

void * LoadObjModel(void* data, unsigned int size, void* param);
void * LoadWbmModel(void* data, unsigned int dataSize, void* param);
void * LoadColladaModel(void* data, unsigned int size, void* param);
C3DModel * LoadDecal(std::string const& path);

struct sOBJLoader
{
	C3DModel * model;
	std::vector<CVector3f> vertices;
	std::vector<CVector2f> textureCoords;
	std::vector<CVector3f> normals;
	std::vector<unsigned int> indexes;
	CMaterialManager materialManager;
	std::vector<sMesh> meshes;
	std::vector<sWeight> weights;
};