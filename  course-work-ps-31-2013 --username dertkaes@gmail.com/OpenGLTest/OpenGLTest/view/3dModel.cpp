#include "3dModel.h"
#include "gl.h"

C3DModel::C3DModel(std::vector<CVector3f> & vertices, std::vector<CVector2f> & textureCoords, std::vector<CVector3f> & normals, std::vector<unsigned int> & indexes,
				   CMaterialManager & materials, std::vector<sMesh> & meshes, std::shared_ptr<IBounding> bounding, float scale)
{
	m_vertices.swap(vertices);
	m_textureCoords.swap(textureCoords);
	m_normals.swap(normals);
	m_indexes.swap(indexes);
	std::swap(m_materials, materials);
	m_meshes.swap(meshes);
	m_bounding = bounding;
	m_scale = scale;
}

void SetMaterial(sMaterial * material)
{
	if(!material)
	{
		return;
	}

	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT, material->ambient);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE, material->diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, material->specular);
	glMaterialf(GL_FRONT,GL_SHININESS, material->shininess);
	CTextureManager * texManager = CTextureManager::GetInstance();
	texManager->SetTexture(material->texture);
}

void C3DModel::Draw(const std::set<std::string> * hideMeshes)
{
	glPushMatrix();
	glScaled(m_scale, m_scale, m_scale);
	if(!m_vertices.empty())
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, &m_vertices[0]);
	}
	if(!m_normals.empty())
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, &m_normals[0]);
	}
	if(!m_textureCoords.empty())
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, &m_textureCoords[0]);
	}
	if(!m_indexes.empty()) //Draw by meshes;
	{
		unsigned int begin = 0;
		unsigned int end;
		for(unsigned int i = 0; i < m_meshes.size(); ++i)
		{
			if(hideMeshes && hideMeshes->find(m_meshes[i].name) != hideMeshes->end())
			{
				begin = (i + 1 == m_meshes.size())?m_indexes.size():m_meshes[i + 1].polygonIndex;
				continue;
			}
			end = m_meshes[i].polygonIndex;
			glDrawElements(GL_TRIANGLES, end - begin, GL_UNSIGNED_INT, &m_indexes[begin]);
			SetMaterial(m_materials.GetMaterial(m_meshes[i].materialName));
			begin = end;
		}
		end = m_indexes.size();
		if(begin != end)
		{
			glDrawElements(GL_TRIANGLES, end - begin, GL_UNSIGNED_INT, &m_indexes[begin]);
		}
	}
	else //Draw in a row
	{
		glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
	}
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	sMaterial empty;
	SetMaterial(&empty);
	glPopMatrix();
}