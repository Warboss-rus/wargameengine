#include "OBJModelFactory.h"

int main(int argc, char* argv[])
{
	if(argc != 2) return 1;
	std::string path(argv[1]);
	C3DModel * model = LoadObjModel(path);
	path = path.substr(0, path.find_last_of('.')) + ".wbm";
	FILE* oFile = fopen(path.c_str(), "wb");
	unsigned int size = model->m_vertices.size() * sizeof(sPoint3);
	fwrite(&size, sizeof(size_t), 1, oFile);
	fwrite(&model->m_vertices[0], size, 1, oFile);
	size = model->m_textureCoords.size() * sizeof(sPoint2);
	fwrite(&size, sizeof(size_t), 1, oFile);
	fwrite(&model->m_textureCoords[0], size, 1, oFile);
	size = model->m_normals.size() * sizeof(sPoint3);
	fwrite(&size, sizeof(size_t), 1, oFile);
	fwrite(&model->m_normals[0], size, 1, oFile);
	size = model->m_indexes.size() * sizeof(size_t);
	fwrite(&size, sizeof(size_t), 1, oFile);
	fwrite(&model->m_indexes[0], size, 1, oFile);
	unsigned int count = model->m_meshes.size();
	fwrite(&count, sizeof(size_t), 1, oFile);
	for(size_t i = 0; i < count; ++i)
	{
		size = model->m_meshes[i].name.size();
		fwrite(&size, sizeof(size_t), 1, oFile);
		fwrite(&model->m_meshes[i].name[0], size, 1, oFile);
		size = model->m_meshes[i].materialName.size();
		fwrite(&size, sizeof(size_t), 1, oFile);
		fwrite(&model->m_meshes[i].materialName[0], size, 1, oFile);
		fwrite(&model->m_meshes[i].polygonIndex, sizeof(size_t), 1, oFile);
	}
	count = model->m_materials.m_materials.size();
	fwrite(&count, sizeof(size_t), 1, oFile);
	for(auto i = model->m_materials.m_materials.begin(); i != model->m_materials.m_materials.end(); ++i)
	{
		size = i->first.size();
		fwrite(&size, sizeof(size_t), 1, oFile);
		fwrite(&i->first, size, 1, oFile);
		fwrite(&i->second.ambient[0], sizeof(float) * 3, 1, oFile);
		fwrite(&i->second.diffuse[0], sizeof(float) * 3, 1, oFile);
		fwrite(&i->second.specular[0], sizeof(float) * 3, 1, oFile);
		fwrite(&i->second.shininess, sizeof(float), 1, oFile);
		size = i->second.texture.size();
		fwrite(&size, sizeof(size_t), 1, oFile);
		fwrite(&i->second.texture[0], size, 1, oFile);
	}
	fclose(oFile);
	return 0;
}