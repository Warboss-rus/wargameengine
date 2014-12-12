#include <string>
#include "3dModel.h"

void * LoadObjModel(void* data, unsigned int size, void* param);
void * LoadWbmModel(void* data, unsigned int dataSize, void* param);
void * LoadColladaModel(void* data, unsigned int size, void* param);

struct sOBJLoader
{
	C3DModel * model;
	std::vector<CVector3f> vertices;
	std::vector<CVector2f> textureCoords;
	std::vector<CVector3f> normals;
	std::vector<unsigned int> indexes;
	CMaterialManager materialManager;
	std::vector<sMesh> meshes;
	std::vector<unsigned int> weightsCount;
	std::vector<unsigned int> weightsIndexes;
	std::vector<float> weights;
	std::vector<sJoint> joints;
	std::vector<sAnimation> animations;
};