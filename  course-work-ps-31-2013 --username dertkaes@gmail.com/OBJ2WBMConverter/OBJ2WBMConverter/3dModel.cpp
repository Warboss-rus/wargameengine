#include "3dModel.h"

C3DModel::C3DModel(std::vector<sPoint3> & vertices, std::vector<sPoint2> & textureCoords, std::vector<sPoint3> & normals, std::vector<unsigned int> & indexes,
				   CMaterialManager & materials, std::vector<sMesh> & meshes, std::shared_ptr<IBounding> bounding, double scale)
{
	m_vertices.swap(vertices);
	m_textureCoords.swap(textureCoords);
	m_normals.swap(normals);
	m_indexes.swap(indexes);
	std::swap(m_materials, materials);
	m_meshes.swap(meshes);
}