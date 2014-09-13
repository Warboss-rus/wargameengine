#include "OBJModelFactory.h"
#include "tinyxml.h"
#include <vector>
#include <map>
#include <string>

std::vector<float> GetFloats(TiXmlElement* data)
{
	std::vector<float> res;
	char * fl = strtok((char*)data->GetText() , " ");
	while (fl != NULL)
	{
		float i = atof(fl);
		res.push_back(i);
		fl = strtok(NULL, " ");
	}
	return res;
}

std::string GetImagePath(std::string const& samplerID, TiXmlElement* effect)
{
	TiXmlElement* newparam = effect->FirstChildElement("newparam");
	while (newparam)
	{
		if (newparam->Attribute("sid") == samplerID)
		{
			TiXmlElement* sampler2D = newparam->FirstChildElement("sampler2D");
			if (sampler2D)
			{
				TiXmlElement* source = sampler2D->FirstChildElement("source");
				if (source)
				{
					std::string surfaceID = source->GetText();
					TiXmlElement* newparam2 = effect->FirstChildElement("newparam");
					while (newparam2)
					{
						if (newparam2->Attribute("sid") == surfaceID)
						{
							TiXmlElement* surface = newparam2->FirstChildElement("surface");
							if (surface)
							{
								TiXmlElement* init_from = surface->FirstChildElement("init_from");
								if (init_from)
								{
									return init_from->GetText();
								}
							}
							newparam2 = newparam2->NextSiblingElement("newparam");
						}
					}

				}
			}
		}
		newparam = newparam->NextSiblingElement("newparam");
	}
}

C3DModel * LoadColladaModel(std::string const& path)
{
	std::vector<CVector3f> newVertices;
	std::vector<CVector2f> newTextureCoords;
	std::vector<CVector3f> newNormals;
	std::vector<unsigned int> indexes;
	CMaterialManager materialManager;
	std::vector<sMesh> meshes;
	TiXmlDocument doc(path.c_str());
	doc.LoadFile();
	TiXmlElement* root = doc.RootElement();
	if (!root)//No root
	{
		return NULL;
	}
	std::map<std::string, std::string> imageTranslator;
	//Process textures
	TiXmlElement* library_images = root->FirstChildElement("library_images");
	if (library_images)
	{
		TiXmlElement* image = library_images->FirstChildElement("image");
		while (image)
		{
			TiXmlElement* init = image->FirstChildElement("init_from");
			if (init)
			{
				imageTranslator[image->Attribute("id")] = init->GetText();
			}
			image = image->NextSiblingElement("image");
		}
	}
	std::map<std::string, std::string> materialTranslator;
	//Materials step 1: Nodes to materials
	TiXmlElement* library_visual_scenes = root->FirstChildElement("library_visual_scenes");
	if (library_visual_scenes)
	{
		TiXmlElement* scene = library_visual_scenes->FirstChildElement("visual_scene");
		while (scene)
		{
			TiXmlElement* node = scene->FirstChildElement("node");
			while (node)
			{
				std::string type = node->Attribute("type");
				if (type == "NODE")
				{
					TiXmlElement* temp = node->FirstChildElement("instance_controller");
					if (!temp) break;
					temp = temp->FirstChildElement("bind_material");
					if (!temp) break;
					temp = temp->FirstChildElement("technique_common");
					if (!temp) break;
					temp = temp->FirstChildElement("instance_material");
					if (!temp) break;
					std::string key = temp->Attribute("symbol");
					std::string value = temp->Attribute("target");
					value.erase(0, 1);
					materialTranslator[key] = value;
				}
				node = node->NextSiblingElement("node");
			}
			scene = scene->NextSiblingElement("visual_scene");
		}
	}
	//Materials step 2: materials to effects
	TiXmlElement* library_materials = root->FirstChildElement("library_materials");
	if (library_materials)
	{
		TiXmlElement* material = library_materials->FirstChildElement("material");
		while (material)
		{
			std::string materialId = material->Attribute("id");
			TiXmlElement* effect = material->FirstChildElement("instance_effect");
			if (effect)
			{
				std::string effectName = effect->Attribute("url");
				effectName.erase(0, 1);
				for (auto i = materialTranslator.begin(); i != materialTranslator.end(); ++i)
				{
					if (i->second == materialId)
					{
						i->second = effectName;
						break;
					}
				}
			}
			material = material->NextSiblingElement("material");
		}
	}
	//Materials step 3: Load effects properties
	TiXmlElement* library_effects = root->FirstChildElement("library_effects");
	if (library_effects)
	{
		TiXmlElement* effect = library_effects->FirstChildElement("effect");
		while (effect)
		{
			sMaterial material;
			TiXmlElement* common = effect->FirstChildElement("profile_COMMON");
			if (common)
			{
				TiXmlElement* technique = common->FirstChildElement("technique");
				while (technique)
				{
					if (std::string(technique->Attribute("sid")) == "common")
					{
						TiXmlElement* phong = technique->FirstChildElement("phong");
						if (!phong) phong = technique->FirstChildElement("blinn");
						if (phong)
						{
							TiXmlElement* ambient = phong->FirstChildElement("ambient");
							if (ambient)
							{
								TiXmlElement* color = ambient->FirstChildElement("color");
								if (color)
								{
									std::vector<float> c = GetFloats(color);
									memcpy(material.ambient, &color[0], 3 * sizeof(float));
								}
							}
							TiXmlElement* diffuse = phong->FirstChildElement("diffuse");
							if (diffuse)
							{
								TiXmlElement* color = diffuse->FirstChildElement("color");
								if (color)
								{
									std::vector<float> c = GetFloats(color);
									memcpy(material.diffuse, &color[0], 3 * sizeof(float));
								}
								TiXmlElement* texture = diffuse->FirstChildElement("texture");
								if (texture)
								{
									material.texture = imageTranslator[GetImagePath(texture->Attribute("texture"), common)];
								}
							}
							TiXmlElement* specular = phong->FirstChildElement("specular");
							if (specular)
							{
								TiXmlElement* color = specular->FirstChildElement("color");
								if (color)
								{
									std::vector<float> c = GetFloats(color);
									memcpy(material.specular, &color[0], 3 * sizeof(float));
								}
							}
							TiXmlElement* shininess = phong->FirstChildElement("shininess");
							if (shininess)
							{
								TiXmlElement* fl = shininess->FirstChildElement("float");
								if (fl) material.shininess = atof(fl->GetText());
							}
						}
					}
					technique = technique->NextSiblingElement("technique");
				}
			}
			TiXmlElement* extra = effect->FirstChildElement("extra");
			if (extra)
			{
				TiXmlElement* technique = extra->FirstChildElement("technique");
				if (technique && std::string(technique->Attribute("profile")) == "FCOLLADA")
				{
					TiXmlElement* userProperties = technique->FirstChildElement("user_properties");
					if (userProperties) material.texture = userProperties->GetText();
				}
			}
			for (auto i = materialTranslator.begin(); i != materialTranslator.end(); ++i)
			{
				if (i->second == effect->Attribute("id"))
				{
					materialManager.AddMaterial(i->first, material);
					break;
				}
			}
			
			effect = effect->NextSiblingElement("effect");
		}
	}
	TiXmlElement* geometry = root->FirstChildElement("library_geometries");//Process geometry
	unsigned int indexOffset = 0;
	TiXmlElement* geometryElement = geometry->FirstChildElement("geometry");
	while (geometryElement != NULL)
	{
		sMesh m;
		m.name = geometryElement->Attribute("name");
		TiXmlElement* mesh = geometryElement->FirstChildElement("mesh");
		while (mesh != NULL)//Parse a mesh
		{
			indexOffset = newVertices.size();
			std::map<std::string, TiXmlElement*> sources;//Parse sources;
			TiXmlElement* source = mesh->FirstChildElement("source");
			while (source != NULL)
			{
				std::string id = source->Attribute("id");
				TiXmlElement* data = source->FirstChildElement("float_array");
				if (!data) data = source->FirstChildElement("int_array");
				sources["#" + id] = data;
				source = source->NextSiblingElement("source");
			}
			TiXmlElement* triangles = mesh->FirstChildElement("triangles");
			if (triangles)
			{
				m.materialName = triangles->Attribute("material");
				m.polygonIndex = indexes.size();
				meshes.push_back(m);
				TiXmlElement* input = triangles->FirstChildElement("input");
				unsigned int vertexOffset = 0;
				unsigned int normalOffset = 0;
				unsigned int texcoordOffset = 0;
				unsigned int maxOffset = 0;
				std::vector<float> vert;
				std::vector<float> normal;
				std::vector<float> texcoord;
				bool simple = true;
				while (input)
				{
					std::string type = input->Attribute("semantic");
					if (type == "VERTEX")
					{
						vertexOffset = atoi(input->Attribute("offset"));
						std::string id = input->Attribute("source");
						id.erase(0, 1);
						TiXmlElement* vertices = mesh->FirstChildElement("vertices");//parse vertices
						if (vertices && vertices->Attribute("id") == id)
						{
							TiXmlElement* vertEntry = vertices->FirstChildElement("input");
							while (vertEntry != NULL)
							{
								std::string type = vertEntry->Attribute("semantic");
								if (type == "POSITION")
								{
									vert = GetFloats(sources[vertEntry->Attribute("source")]);
								}
								else if (type == "NORMAL")
								{
									normal = GetFloats(sources[vertEntry->Attribute("source")]);
								}
								else if (type == "TEXCOORD")
								{
									texcoord = GetFloats(sources[vertEntry->Attribute("source")]);
								}
								vertEntry = vertEntry->NextSiblingElement("input");
							}
						}
					}
					else if (type == "NORMAL")
					{
						simple = false;
						normalOffset = atoi(input->Attribute("offset"));
						normal = GetFloats(sources[input->Attribute("source")]);
					}
					else if (type == "TEXCOORD")
					{
						texcoordOffset = atoi(input->Attribute("offset"));
						simple = false;
						texcoord = GetFloats(sources[input->Attribute("source")]);
					}
					if (atoi(input->Attribute("offset")) > maxOffset) maxOffset = atoi(input->Attribute("offset"));
					input = input->NextSiblingElement("input");
				}
				maxOffset++;
				if (simple)
				{
					unsigned int oldSize = newVertices.size();
					newVertices.resize(oldSize + vert.size() / 3);
					memcpy(&newVertices[oldSize], &vert[0], vert.size() * sizeof(float));
					oldSize = newNormals.size();
					newNormals.resize(oldSize + normal.size() / 3);
					memcpy(&newNormals[oldSize], &normal[0], normal.size() * sizeof(float));
					oldSize = newTextureCoords.size();
					newTextureCoords.resize(oldSize + texcoord.size() / 2);
					memcpy(&newTextureCoords[oldSize], &texcoord[0], texcoord.size() * sizeof(float));
				}
				TiXmlElement* data = triangles->FirstChildElement("p");
				std::vector<unsigned int> index;
				char* result = strtok((char*)data->GetText(), " ");
				while (result != NULL)
				{
					int i = atoi(result);
					if (simple)
					{
						indexes.push_back(i + indexOffset);
					}
					else
					{
						index.push_back(i);
						if (index.size() == maxOffset)
						{
							newVertices.push_back(CVector3f(vert[index[vertexOffset] * 3], vert[index[vertexOffset] * 3 + 1], vert[index[vertexOffset] * 3 + 2]));
							newNormals.push_back(CVector3f(normal[index[normalOffset] * 3], normal[index[normalOffset] * 3 + 1], normal[index[normalOffset] * 3 + 2]));
							newTextureCoords.push_back(CVector2f(texcoord[index[texcoordOffset] * 2], texcoord[index[texcoordOffset] * 2 + 1]));
							indexes.push_back(newVertices.size() - 1);
							index.clear();
						}
					}
					result = strtok(NULL, " ");
				}
			}
			mesh = mesh->NextSiblingElement("mesh");
		}
		geometryElement = geometryElement->NextSiblingElement("geometry");
	}
	doc.Clear();
	return new C3DModel(newVertices, newTextureCoords, newNormals, indexes, materialManager, meshes);
}