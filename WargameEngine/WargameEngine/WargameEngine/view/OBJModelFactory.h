#include <string>
#include "3dModel.h"

struct sOBJLoader
{
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

void LoadObjModel(void* data, unsigned int size, sOBJLoader & loader);
void LoadWbmModel(void* data, unsigned int dataSize, sOBJLoader & loader);
void LoadColladaModel(void* data, unsigned int size, sOBJLoader & loader);