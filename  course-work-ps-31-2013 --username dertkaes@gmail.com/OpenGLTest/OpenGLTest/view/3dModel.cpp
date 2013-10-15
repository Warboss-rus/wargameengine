#include "3dModel.h"
#include <GL\glut.h>

void SetMaterial(sMaterial * material)
{
	if(!material)
	{
		return;
	}
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,material->ambient);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,material->diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,material->specular);
	glMaterialf(GL_FRONT,GL_SHININESS,material->shininess);
	CTextureManager * texManager = CTextureManager::GetInstance();
	texManager->SetTexture(material->texture);
}

void C3DModel::Draw()
{
	if(!m_vertices.empty())
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_DOUBLE, 0, &m_vertices[0]);
	}
	if(!m_normals.empty())
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_DOUBLE, 0, &m_normals[0]);
	}
	if(!m_textureCoords.empty())
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_DOUBLE, 0, &m_textureCoords[0]);
	}
	if(!m_indexes.empty()) //Draw by indexes;
	{
		unsigned int begin = 0;
		unsigned int end;
		for(unsigned int i = 0; i < m_usedMaterials.size(); ++i)
		{
			end = m_usedMaterials[i].polygonIndex;
			glDrawElements(GL_TRIANGLES, end - begin, GL_UNSIGNED_INT, &m_indexes[begin]);
			SetMaterial(m_materials.GetMaterial(m_usedMaterials[i].materialName));
			begin = end;
		}
		end = m_indexes.size();
		glDrawElements(GL_TRIANGLES, end - begin, GL_UNSIGNED_INT, &m_indexes[begin]);
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
}