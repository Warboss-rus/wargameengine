#include "MaterialManager.h"
#include <fstream>
#include <string>
#include <sstream>
#include <GL\glut.h>

unsigned int LoadBMPTexture(std::string const& path);

sMaterial * CMaterialManager::GetMaterial(std::string const& name)
{
	if(m_materials.find(name) == m_materials.end())
	{
		return NULL;
	}
	sMaterial * material = m_materials[name];
	return material;
}

CMaterialManager::~CMaterialManager()
{
	for(auto i = m_materials.begin(); i != m_materials.end(); ++i)
	{
		delete i->second;
	}
}

void CMaterialManager::LoadMTL(std::string const& path)
{
	std::ifstream iFile(path);
	std::string line;
	std::string type;
	float dvalue;
	sMaterial * lastMaterial = NULL;
	while(std::getline(iFile, line))
	{
		if(line.empty() || line[0] == '#')//Empty line or commentary
			continue;

		std::istringstream lineStream(line);
		lineStream >> type;

		if(type == "newmtl") //name
		{
			lineStream >> type;
			lastMaterial = new sMaterial;
			m_materials[type] = lastMaterial;
		}

		if(type == "Ka") //ambient color
		{
			for (unsigned int i = 0; i < 3; ++i)
			{
				lineStream >> dvalue;
				lastMaterial->ambient[i] = dvalue;
			}
		}
		if(type == "Kd") //diffuse color
		{
			for (unsigned int i = 0; i < 3; ++i)
			{
				lineStream >> dvalue;
				lastMaterial->diffuse[i] = dvalue;
			}
		}
		if(type == "Ks") //specular color
		{
			for (unsigned int i = 0; i < 3; ++i)
			{
				lineStream >> dvalue;
				lastMaterial->specular[i] = dvalue;
			}
		}
		if(type == "Ns") //specular coefficient
		{
			lineStream >> dvalue;
			lastMaterial->shininess = dvalue;
		}
		if(type == "map_Kd") //texture
		{
			std::string path;
			lineStream >> path;
			lastMaterial->texture = path;
		}
	}
	iFile.close();
}