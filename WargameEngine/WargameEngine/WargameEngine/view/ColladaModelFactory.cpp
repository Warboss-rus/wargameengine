#include "OBJModelFactory.h"
#include "../tinyxml.h"
#include <vector>
#include <map>
#include <string>
#include "../LogWriter.h"

std::vector<float> GetFloats(TiXmlElement* data)
{
	std::vector<float> res;
	char * fl = strtok((char*)data->GetText() , " \n\t");
	while (fl != NULL)
	{
		for (unsigned int i = 0; i < strlen(fl); ++i)
		{
			if (fl[i] == ',') fl[i] = '.';
		}
		float i = atof(fl);
		res.push_back(i);
		fl = strtok(NULL, " \n\t");
	}
	return res;
}

std::vector<unsigned int> GetUIntegers(TiXmlElement* data)
{
	std::vector<unsigned int> res;
	char * fl = strtok((char*)data->GetText(), " \n\t");
	while (fl != NULL)
	{
		unsigned int i = atoi(fl);
		res.push_back(i);
		fl = strtok(NULL, " \n\t");
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
	return samplerID;
}

void LoadJoints(TiXmlElement * element, std::vector<sJoint> & arr, int parent)
{
	sJoint joint;
	joint.name = element->Attribute("name");
	joint.bone = element->Attribute("sid");
	joint.parentIndex = parent;
	TiXmlElement * matrix = element->FirstChildElement("matrix");
	if (matrix)
	{
		std::vector<float> mat = GetFloats(matrix);
		memcpy(joint.matrix, &mat[0], sizeof(float) * 4);
	}
	else//todo: matrix from translation and rotation
	{
		float resultMatrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
		TiXmlElement * translate = element->FirstChildElement("translate");
		std::vector<float> translatef = GetFloats(translate);
		//resultMatrix *= translatef
		TiXmlElement * rotate = element->FirstChildElement("rotate");
		while (rotate)
		{
			std::vector<float> rotatef = GetFloats(rotate);
			//resultMatrix *= rotatef
			rotate = rotate->NextSiblingElement("rotate");
		}
	}
	int index = arr.size();
	arr.push_back(joint);
	TiXmlElement * child = element->FirstChildElement("node");
	while (child)
	{
		if (child->Attribute("type") == std::string("JOINT"))
		{
			LoadJoints(child, arr, index);
		}
		child = child->NextSiblingElement("node");
	}
}

void * LoadColladaModel(void* data, unsigned int size, void* param)
{
	sOBJLoader * loader = (sOBJLoader*)param;
	TiXmlDocument doc;
	doc.Parse((const char*)data);
	TiXmlElement* root = doc.RootElement();
	if (!root)//No root
	{
		CLogWriter::WriteLine("Cannot load model. No root.");
		return loader;
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
	std::vector<sJoint> joints;
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
				if (node->Attribute("type") == std::string("NODE"))
				{
					TiXmlElement* temp = node->FirstChildElement("instance_controller");
					if (!temp) temp = node->FirstChildElement("instance_geometry");
					if (temp)
					{
						temp = temp->FirstChildElement("bind_material");
						if (!temp) break;
						temp = temp->FirstChildElement("technique_common");
						if (!temp) break;
						TiXmlElement * material = temp->FirstChildElement("instance_material");
						while (material)
						{
							std::string key = material->Attribute("symbol");
							std::string value = material->Attribute("target");
							value.erase(0, 1);
							materialTranslator[key] = value;
							material = material->NextSiblingElement("instance_material");
						}
					}
				}
				else if (node->Attribute("type") == std::string("JOINT"))
				{
					LoadJoints(node, joints, -1);
				}
				node = node->NextSiblingElement("node");
			}
			scene = scene->NextSiblingElement("visual_scene");
		}
	}
	else
	{
		CLogWriter::WriteLine("Model loading warning. No visual scenes found.");
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
	else
	{
		CLogWriter::WriteLine("Model loading warning. No materials found.");
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
					std::string sid = technique->Attribute("sid");
					if (sid == "common" || sid == "standard")
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
									std::string image = texture->Attribute("texture");
									material.texture = imageTranslator[GetImagePath(image, common)];
									if (material.texture.size() > 7 && (material.texture.substr(0, 7) == "file://" || material.texture.substr(0, 7) == "file:\\\\"))
									{
										material.texture.erase(0, 7);
									}
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
					loader->materialManager.AddMaterial(i->first, material);
					break;
				}
			}
			
			effect = effect->NextSiblingElement("effect");
		}
	}
	else
	{
		CLogWriter::WriteLine("Model loading warning. No effects found.");
	}
	//Animation step1: Process weights
	TiXmlElement* controllerLib = root->FirstChildElement("library_controllers");
	std::vector<sWeight> weights;
	TiXmlElement* controller = controllerLib->FirstChildElement("controller");
	while (controller != NULL)
	{
		TiXmlElement * skin = controller->FirstChildElement("skin");
		if (skin)
		{
			std::map<std::string, TiXmlElement*> sources;
			TiXmlElement * source = skin->FirstChildElement("source");
			while (source)
			{
				TiXmlElement* data = source->FirstChildElement("float_array");
				if (!data) data = source->FirstChildElement("Name_array");
				sources[source->Attribute("id")] = data;
				source = source->NextSiblingElement("source");
			}
			TiXmlElement * j = skin->FirstChildElement("joints");
			TiXmlElement * jointSource = NULL;
			TiXmlElement * invMatrices = NULL;
			if (j)
			{
				TiXmlElement * input = j->FirstChildElement("input");
				while (input)
				{
					if (input->Attribute("semantic") == std::string("JOINT"))
					{
						jointSource = sources[std::string(input->Attribute("source")).substr(1)];
					}
					if (input->Attribute("semantic") == std::string("INV_BIND_MATRIX"))
					{
						invMatrices = sources[std::string(input->Attribute("source")).substr(1)];
					}
					input = input->NextSiblingElement("input");
				}
			}
			if (jointSource && invMatrices)//Assing inv_bind_matricies
			{
				std::vector<float> inv = GetFloats(invMatrices);
				unsigned int index = 0;
				char * fl = strtok((char*)jointSource->GetText(), " \n\t");
				while (fl != NULL)
				{
					for (unsigned int i = 0; i < joints.size(); ++i)
					{
						if (joints[i].name == fl)
						{
							memcpy(joints[i].invBindMatrix, &inv[index * 16], sizeof(float) * 16);
						}
					}
					index++;
					fl = strtok(NULL, " \n\t");
				}
			}
			TiXmlElement * vertex_weights = skin->FirstChildElement("vertex_weights");
			if (vertex_weights)
			{
				TiXmlElement * weightsSource = NULL;
				TiXmlElement * input = vertex_weights->FirstChildElement("input");
				while (input)
				{
					if (input->Attribute("semantic") == std::string("JOINT") && !jointSource)
					{
						jointSource = sources[std::string(input->Attribute("source")).substr(1)];
					}
					if (input->Attribute("semantic") == std::string("WEIGHT"))
					{
						weightsSource = sources[std::string(input->Attribute("source")).substr(1)];
					}
					input = input->NextSiblingElement("input");
				}
				std::vector<unsigned int> vcount = GetUIntegers(vertex_weights->FirstChildElement("vcount"));
				std::vector<unsigned int> v = GetUIntegers(vertex_weights->FirstChildElement("v"));
				int j = 0;
				for (unsigned int i = 0; i < vcount.size(); ++i)
				{
					sWeight weight;
					for (unsigned int k = 0; k < vcount[i]; k++)
					{
						weight.indexes.push_back(j + k * 2);
						weight.weights.push_back(j + k * 2 + 1);
					}
					j += vcount[i] * 2;
				}
				vertex_weights = vertex_weights->NextSiblingElement("vertex_weights");
			}
		}
		controller = controller->NextSiblingElement("controller");
	}
	//Geometry: Process geometry
	TiXmlElement* geometry = root->FirstChildElement("library_geometries");
	unsigned int indexOffset = 0;
	TiXmlElement* geometryElement = geometry->FirstChildElement("geometry");
	while (geometryElement != NULL)
	{
		sMesh m;
		m.name = geometryElement->Attribute("name");
		TiXmlElement* mesh = geometryElement->FirstChildElement("mesh");
		while (mesh != NULL)//Parse a mesh
		{
			indexOffset = loader->vertices.size();
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
			if (!triangles) triangles = mesh->FirstChildElement("polygons");
			if (triangles)
			{
				m.materialName = triangles->Attribute("material");
				m.polygonIndex = loader->indexes.size();
				loader->meshes.push_back(m);
				TiXmlElement* input = triangles->FirstChildElement("input");
				unsigned int vertexOffset = 0;
				unsigned int normalOffset = 0;
				unsigned int texcoordOffset = 0;
				int maxOffset = 0;
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
					unsigned int oldSize = loader->vertices.size();
					loader->vertices.resize(oldSize + vert.size() / 3);
					memcpy(&loader->vertices[oldSize], &vert[0], vert.size() * sizeof(float));
					oldSize = loader->normals.size();
					loader->normals.resize(oldSize + normal.size() / 3);
					memcpy(&loader->normals[oldSize], &normal[0], normal.size() * sizeof(float));
					oldSize = loader->textureCoords.size();
					loader->textureCoords.resize(oldSize + texcoord.size() / 2);
					memcpy(&loader->textureCoords[oldSize], &texcoord[0], texcoord.size() * sizeof(float));
					//save weights
				}
				std::vector<unsigned int> indexes;
				TiXmlElement* data = triangles->FirstChildElement("p");
				while (data)
				{
					char* result = strtok((char*)data->GetText(), " \n\t");
					while (result != NULL)
					{
						int i = atoi(result);
						if (simple)
						{
							loader->indexes.push_back(i + indexOffset);
						}
						else
						{
							indexes.push_back(i);
							if (indexes.size() == maxOffset)
							{
								loader->vertices.push_back(CVector3f(vert[indexes[vertexOffset] * 3], vert[indexes[vertexOffset] * 3 + 1], vert[indexes[vertexOffset] * 3 + 2]));
								loader->normals.push_back(CVector3f(normal[indexes[normalOffset] * 3], normal[indexes[normalOffset] * 3 + 1], normal[indexes[normalOffset] * 3 + 2]));
								loader->textureCoords.push_back(CVector2f(texcoord[indexes[texcoordOffset] * 2], texcoord[indexes[texcoordOffset] * 2 + 1]));
								loader->indexes.push_back(loader->vertices.size() - 1);
								//save weights
								indexes.clear();
							}
						}
						result = strtok(NULL, " \n\t");
					}
					data = data->NextSiblingElement("p");
				}
			}
			mesh = mesh->NextSiblingElement("mesh");
		}
		geometryElement = geometryElement->NextSiblingElement("geometry");
	}
	doc.Clear();
	return loader;
}