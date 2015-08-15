#include "OBJModelFactory.h"
#include <vector>
#include <map>
#include <string>
#include "../LogWriter.h"
#include <algorithm>
#include "../rapidxml/rapidxml.hpp"
#include <sstream>
#include <numeric>

using namespace std;
using namespace rapidxml;

template<class T>
vector<T> GetValues(xml_node<>* data)
{
	vector<T> res;
	//replace(value.begin(), value.end(), ',', '.');
	stringstream sstream(data->value());
	while (!sstream.eof())
	{
		T val;
		sstream >> val;
		res.push_back(val);
	}
	res.shrink_to_fit();
	return res;
}

string GetImagePath(string const& samplerID, xml_node<>* effect)
{
	xml_node<>* newparam = effect->first_node("newparam");
	while (newparam)
	{
		if (newparam->first_attribute("sid")->value() == samplerID)
		{
			xml_node<>* sampler2D = newparam->first_node("sampler2D");
			if (sampler2D)
			{
				xml_node<>* source = sampler2D->first_node("source");
				if (source)
				{
					string surfaceID = source->value();
					xml_node<>* newparam2 = effect->first_node("newparam");
					while (newparam2)
					{
						if (newparam2->first_attribute("sid")->value() == surfaceID)
						{
							xml_node<>* surface = newparam2->first_node("surface");
							if (surface)
							{
								xml_node<>* init_from = surface->first_node("init_from");
								if (init_from)
								{
									return init_from->value();
								}
							}
							newparam2 = newparam2->next_sibling("newparam");
						}
					}

				}
			}
		}
		newparam = newparam->next_sibling("newparam");
	}
	return samplerID;
}

void LoadJoints(xml_node<> * element, vector<sJoint> & arr, int parent)
{
	sJoint joint;
	joint.bone = element->first_attribute("sid")->value();
	joint.id = element->first_attribute("id")->value();
	joint.parentIndex = parent;
	xml_node<> * matrix = element->first_node("matrix");
	if (matrix)
	{
		vector<float> mat = GetValues<float>(matrix);
		memcpy(joint.matrix, mat.data(), sizeof(float) * 16);
	}
	else//todo: matrix from translation and rotation
	{
		float resultMatrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
		xml_node<> * translate = element->first_node("translate");
		vector<float> translatef = GetValues<float>(translate);
		//resultMatrix *= translatef
		xml_node<> * rotate = element->first_node("rotate");
		while (rotate)
		{
			vector<float> rotatef = GetValues<float>(rotate);
			//resultMatrix *= rotatef
			rotate = rotate->next_sibling("rotate");
		}
	}
	int index = arr.size();
	arr.push_back(joint);
	xml_node<> * child = element->first_node("node");
	while (child)
	{
		if (child->first_attribute("type")->value() == string("JOINT"))
		{
			LoadJoints(child, arr, index);
		}
		child = child->next_sibling("node");
	}
}

void LoadAnimations(xml_node<> * element, vector<sJoint> const& joints, vector<sAnimation> & anims, int parent)
{
	xml_node<>* animation = element->first_node("animation");
	while (animation)
	{
		map<string, xml_node<>*> sources;//Parse sources;
		xml_node<>* source = animation->first_node("source");
		while (source != NULL)
		{
			string id = source->first_attribute("id")->value();
			xml_node<>* data = source->first_node("float_array");
			//if (!data) data = source->first_node("Name_array");
			sources["#" + id] = data;
			source = source->next_sibling("source");
		}
		sAnimation anim;
		anim.id = animation->first_attribute("id")->value();
		xml_node<>* sampler = animation->first_node("sampler");
		if (sampler)
		{
			xml_node<>* input = sampler->first_node("input");
			while (input)
			{
				if (input->first_attribute("semantic")->value() == string("INPUT"))
				{
					anim.keyframes = GetValues<float>(sources[input->first_attribute("source")->value()]);
				}
				if (input->first_attribute("semantic")->value() == string("OUTPUT"))
				{
					anim.matrices = GetValues<float>(sources[input->first_attribute("source")->value()]);
				}
				input = input->next_sibling("input");
			}
		}
		xml_node<>* channel = animation->first_node("channel");
		while (channel)
		{
			string bone = channel->first_attribute("target")->value();
			string mode = bone.substr(bone.find('/') + 1);
			bone = bone.substr(0, bone.find('/'));
			for (size_t i = 0; i < joints.size(); ++i)
			{
				if (bone == joints[i].id)
				{
					if (mode != "transform")
					{
						vector<float> result;
						if (mode.substr(0, 9) == "transform")
						{
							char x = mode[10] - '0';
							char y = mode[13] - '0';
							float matrix[16];
							memcpy(matrix, joints[i].matrix, sizeof(float) * 16);
							for (size_t j = 0; j < anim.matrices.size(); ++j)
							{
								matrix[y * 4 + x] = anim.matrices[j];
								for (size_t k = 0; k < 16; ++k)
								{
									result.push_back(matrix[k]);
								}
							}
						}
						anim.matrices = result;
					}
					anim.boneIndex = i;
					anim.duration = 0.0f;
					for (size_t j = 0; j < anim.keyframes.size(); ++j)
					{
						if (anim.keyframes[j] > anim.duration)
						{
							anim.duration = anim.keyframes[j];
						}
					}
					anims.push_back(anim);
					if (parent != -1)
						anims[parent].children.push_back(anims.size() - 1);
					break;
				}
			}
			channel = channel->next_sibling("channel");
		}
		LoadAnimations(animation, joints, anims, anims.size() - 1);
		animation = animation->next_sibling("animation");
	}
}

void LoadVisualScenes(xml_node<>* library_visual_scenes, map<string, string> &materialTranslator, sOBJLoader &loader)
{
	xml_node<>* scene = library_visual_scenes->first_node("visual_scene");
	while (scene)
	{
		xml_node<>* node = scene->first_node("node");
		while (node)
		{
			if (node->first_attribute("type") == NULL || node->first_attribute("type")->value() == string("NODE"))
			{
				xml_node<>* temp = node->first_node("instance_controller");
				if (!temp) temp = node->first_node("instance_geometry");
				if (temp)
				{
					temp = temp->first_node("bind_material");
					if (!temp) break;
					temp = temp->first_node("technique_common");
					if (!temp) break;
					xml_node<> * material = temp->first_node("instance_material");
					while (material)
					{
						string key = material->first_attribute("symbol")->value();
						string value = material->first_attribute("target")->value();
						value.erase(0, 1);
						materialTranslator[key] = value;
						material = material->next_sibling("instance_material");
					}
				}
			}
			else if (node->first_attribute("type")->value() == string("JOINT"))
			{
				LoadJoints(node, loader.joints, -1);
			}
			node = node->next_sibling("node");
		}
		scene = scene->next_sibling("visual_scene");
	}
}

void LoadMaterials(xml_node<>* library_materials, map<string, string> &materialTranslator)
{
	xml_node<>* material = library_materials->first_node("material");
	while (material)
	{
		string materialId = material->first_attribute("id")->value();
		xml_node<>* effect = material->first_node("instance_effect");
		if (effect)
		{
			string effectName = effect->first_attribute("url")->value();
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
		material = material->next_sibling("material");
	}
}

void LoadPhongModel(xml_node<>* phong, sMaterial &material, map<string, string>& imageTranslator, xml_node<>* common)
{
	xml_node<>* ambient = phong->first_node("ambient");
	if (ambient)
	{
		xml_node<>* color = ambient->first_node("color");
		if (color)
		{
			vector<float> c = GetValues<float>(color);
			memcpy(material.ambient, c.data(), 3 * sizeof(float));
		}
	}
	xml_node<>* diffuse = phong->first_node("diffuse");
	if (diffuse)
	{
		xml_node<>* color = diffuse->first_node("color");
		if (color)
		{
			vector<float> c = GetValues<float>(color);
			memcpy(material.diffuse, c.data(), 3 * sizeof(float));
		}
		xml_node<>* texture = diffuse->first_node("texture");
		if (texture)
		{
			string image = texture->first_attribute("texture")->value();
			material.texture = imageTranslator[GetImagePath(image, common)];
			if (material.texture.size() > 7 && ((material.texture.substr(0, 7) == "file://" || material.texture.substr(0, 7) == "file:\\\\")))
			{
				material.texture.erase(0, 8);
			}
		}
	}
	xml_node<>* specular = phong->first_node("specular");
	if (specular)
	{
		xml_node<>* color = specular->first_node("color");
		if (color)
		{
			vector<float> c = GetValues<float>(color);
			memcpy(material.specular, c.data(), 3 * sizeof(float));
		}
	}
	xml_node<>* shininess = phong->first_node("shininess");
	if (shininess)
	{
		xml_node<>* fl = shininess->first_node("float");
		if (fl) material.shininess = static_cast<float>(atof(fl->value()));
	}
}

void LoadEffectsLibrary(xml_node<>* library_effects, map<string, string>& imageTranslator, map<string, string> &materialTranslator, sOBJLoader &loader)
{
	xml_node<>* effect = library_effects->first_node("effect");
	while (effect)
	{
		sMaterial material;
		xml_node<>* common = effect->first_node("profile_COMMON");
		if (common)
		{
			xml_node<>* technique = common->first_node("technique");
			while (technique)
			{
				string sid = technique->first_attribute("sid")->value();
				transform(sid.begin(), sid.end(), sid.begin(), ::tolower);
				if (sid == "common" || sid == "standard")
				{
					xml_node<>* phong = technique->first_node("phong");
					if (!phong) phong = technique->first_node("blinn");
					if (phong)
					{
						LoadPhongModel(phong, material, imageTranslator, common);
					}
				}
				technique = technique->next_sibling("technique");
			}
		}
		xml_node<>* extra = effect->first_node("extra");
		if (extra)
		{
			xml_node<>* technique = extra->first_node("technique");
			if (technique && string(technique->first_attribute("profile")->value()) == "FCOLLADA")
			{
				xml_node<>* userProperties = technique->first_node("user_properties");
				if (userProperties) material.texture = userProperties->value();
			}
		}
		for (auto i = materialTranslator.begin(); i != materialTranslator.end(); ++i)
		{
			if (i->second == effect->first_attribute("id")->value())
			{
				loader.materialManager.AddMaterial(i->first, material);
				break;
			}
		}

		effect = effect->next_sibling("effect");
	}
}

void LoadAnimationClips(xml_node<>* clipsLib, sOBJLoader &loader)
{
	xml_node<>* clip = clipsLib->first_node("animation_clip");
	while (clip)
	{
		sAnimation anim;
		anim.id = clip->first_attribute("id")->value();
		anim.boneIndex = 0;
		anim.duration = static_cast<float>(atof(clip->first_attribute("end")->value()) - atof(clip->first_attribute("start")->value()));
		xml_node<>* animation = clip->first_node("instance_animation");
		while (animation)
		{
			string animName = animation->first_attribute("url")->value() + 1;
			for (size_t i = 0; i < loader.animations.size(); ++i)
			{
				if (loader.animations[i].id == animName)
				{
					anim.children.push_back(i);
					break;
				}
			}
			animation = animation->next_sibling("instance_animation");
		}
		if (anim.children.size() > 0)
		{
			loader.animations.push_back(anim);
		}
		clip = clip->next_sibling("animation_clip");
	}
}

void LoadColladaModel(void* data, unsigned int size, sOBJLoader & loader)
{
	std::vector<char> normalizedData;
	normalizedData.resize(size);
	memcpy(normalizedData.data(), data, size);
	normalizedData.push_back('\0');
	xml_document<> doc;
	doc.parse<parse_trim_whitespace>(normalizedData.data());
	xml_node<>* root = doc.first_node();
	if (!root)//No root
	{
		LogWriter::WriteLine("Cannot load model. No root.");
		return;
	}
	map<string, string> imageTranslator;
	//Process textures
	xml_node<>* library_images = root->first_node("library_images");
	if (library_images)
	{
		xml_node<>* image = library_images->first_node("image");
		while (image)
		{
			xml_node<>* init = image->first_node("init_from");
			if (init)
			{
				imageTranslator[image->first_attribute("id")->value()] = init->value();
			}
			image = image->next_sibling("image");
		}
	}
	map<string, string> materialTranslator;
	//Materials step 1: Nodes to materials
	xml_node<>* library_visual_scenes = root->first_node("library_visual_scenes");
	if (library_visual_scenes)
	{
		LoadVisualScenes(library_visual_scenes, materialTranslator, loader);
	}
	else
	{
		LogWriter::WriteLine("Model loading warning. No visual scenes found.");
	}
	//Materials step 2: materials to effects
	xml_node<>* library_materials = root->first_node("library_materials");
	if (library_materials)
	{
		LoadMaterials(library_materials, materialTranslator);
	}
	else
	{
		LogWriter::WriteLine("Model loading warning. No materials found.");
	}
	//Materials step 3: Load effects properties
	xml_node<>* library_effects = root->first_node("library_effects");
	if (library_effects)
	{
		LoadEffectsLibrary(library_effects, imageTranslator, materialTranslator, loader);
	}
	else
	{
		LogWriter::WriteLine("Model loading warning. No effects found.");
	}
	//Animation step1: Process weights
	map<string, vector<unsigned int>> weightCount;
	map<string, vector<unsigned int>> weightIndexes;
	map<string, vector<float>> weights;
	map<string, vector<float>> bindShapeMatrices;
	xml_node<>* controllerLib = root->first_node("library_controllers");
	if (controllerLib)
	{
		xml_node<>* controller = controllerLib->first_node("controller");
		while (controller != NULL)
		{
			xml_node<> * skin = controller->first_node("skin");
			if (skin)
			{
				bindShapeMatrices[skin->first_attribute("source")->value() + 1] = GetValues<float>(skin->first_node("bind_shape_matrix"));
				string geometryId = skin->first_attribute("source")->value() + 1;
				map<string, xml_node<>*> sources;
				xml_node<> * source = skin->first_node("source");
				while (source)
				{
					xml_node<>* sourceData = source->first_node("float_array");
					if (!sourceData) sourceData = source->first_node("Name_array");
					sources[source->first_attribute("id")->value()] = sourceData;
					source = source->next_sibling("source");
				}
				xml_node<> * j = skin->first_node("joints");
				xml_node<> * jointSource = NULL;
				xml_node<> * invMatrices = NULL;
				if (j)
				{
					xml_node<> * input = j->first_node("input");
					while (input)
					{
						if (input->first_attribute("semantic")->value() == string("JOINT"))
						{
							jointSource = sources[input->first_attribute("source")->value() + 1];
						}
						if (input->first_attribute("semantic")->value() == string("INV_BIND_MATRIX"))
						{
							invMatrices = sources[input->first_attribute("source")->value() + 1];
						}
						input = input->next_sibling("input");
					}
				}
				if (jointSource && invMatrices)//assign inv_bind_matricies
				{
					vector<float> inv = GetValues<float>(invMatrices);
					unsigned int index = 0;
					stringstream sstream(jointSource->value());
					while (sstream.good())
					{
						string fl;
						sstream >> fl;
						auto it = find_if(loader.joints.begin(), loader.joints.end(), [&](sJoint const& joint) {return joint.bone == fl;});
						if(it != loader.joints.end())
						{
							memcpy(it->invBindMatrix, &inv[index * 16], sizeof(float) * 16);
						}
						index++;
					}
				}
				xml_node<> * vertex_weights = skin->first_node("vertex_weights");
				if (vertex_weights)
				{
					xml_node<> * weightsSource = NULL;
					xml_node<> * input = vertex_weights->first_node("input");
					while (input)
					{
						if (input->first_attribute("semantic")->value() == string("JOINT") && !jointSource)
						{
							jointSource = sources[input->first_attribute("source")->value() + 1];
						}
						if (input->first_attribute("semantic")->value() == string("WEIGHT"))
						{
							weightsSource = sources[input->first_attribute("source")->value() + 1];
						}
						input = input->next_sibling("input");
					}
					vector<unsigned int> vcount = GetValues<unsigned int>(vertex_weights->first_node("vcount"));
					vector<unsigned int> v = GetValues<unsigned int>(vertex_weights->first_node("v"));
					vector<string> jointNames;
					string sname = jointSource->first_node()->value();
					replace(sname.begin(), sname.end(), '\0', ' ');
					stringstream sstream(sname);
					while (!sstream.eof())
					{
						string val;
						sstream >> val;
						jointNames.push_back(val);
					}
					vector<float> weightArray = GetValues<float>(weightsSource);
					size_t l = 0;
					weightCount[geometryId] = vcount;
					for (size_t i = 0; i < vcount.size(); ++i)
					{
						for (size_t k = 0; k < vcount[i]; k++)
						{
							unsigned int jointIndex = v[l + k * 2];
							if (jointIndex >= jointNames.size())
							{
								//LogWriter::WriteLine("Model loading warning. Error parsing joints information");
								continue;
							}
							unsigned int weightIndex = v[l + k * 2 + 1];
							for (size_t m = 0; m < loader.joints.size(); ++m)
							{
								if (loader.joints[m].bone == jointNames[jointIndex])
								{
									weightIndexes[geometryId].push_back(m);
									weights[geometryId].push_back(weightArray[weightIndex]);
									break;
								}
							}
						}
						l += vcount[i] * 2;
					}
					vertex_weights = vertex_weights->next_sibling("vertex_weights");
				}
			}
			controller = controller->next_sibling("controller");
		}
	}
	//Load Animations
	xml_node<>* animationLib = root->first_node("library_animations");
	if (animationLib && controllerLib)
	{
		LoadAnimations(animationLib, loader.joints, loader.animations, -1);
	}
	xml_node<>* clipsLib = root->first_node("library_animation_clips");
	if (clipsLib)
	{
		LoadAnimationClips(clipsLib, loader);
	}
	//Geometry: Process geometry
	xml_node<>* geometry = root->first_node("library_geometries");
	unsigned int indexOffset = 0;
	xml_node<>* geometryElement = geometry->first_node("geometry");
	while (geometryElement != NULL)
	{
		sMesh m;
		string meshId = geometryElement->first_attribute("id")->value();
		if (geometryElement->first_attribute("name"))
			m.name = geometryElement->first_attribute("name")->value();
		else
			m.name = meshId;
		xml_node<>* mesh = geometryElement->first_node("mesh");
		while (mesh != NULL)//Parse a mesh
		{
			indexOffset = loader.vertices.size();
			map<string, xml_node<>*> sources;//Parse sources;
			xml_node<>* source = mesh->first_node("source");
			while (source != NULL)
			{
				string id = source->first_attribute("id")->value();
				xml_node<>* sourceData = source->first_node("float_array");
				if (!sourceData) sourceData = source->first_node("int_array");
				sources["#" + id] = sourceData;
				source = source->next_sibling("source");
			}
			xml_node<>* triangles = mesh->first_node("triangles");
			if (!triangles) triangles = mesh->first_node("polygons");
			if (!triangles) triangles = mesh->first_node("polylist");
			if (triangles)
			{
				m.materialName = triangles->first_attribute("material")->value();
				m.polygonIndex = loader.indexes.size();
				loader.meshes.push_back(m);
				xml_node<>* input = triangles->first_node("input");
				unsigned int vertexOffset = 0;
				unsigned int normalOffset = 0;
				unsigned int texcoordOffset = 0;
				size_t maxOffset = 0;
				vector<float> vert;
				vector<float> normal;
				vector<float> texcoord;
				unsigned int texCoordStride = 2;
				bool simple = true;
				while (input)
				{
					string type = input->first_attribute("semantic")->value();
					if (type == "VERTEX")
					{
						vertexOffset = atoi(input->first_attribute("offset")->value());
						string id = input->first_attribute("source")->value();
						id.erase(0, 1);
						xml_node<>* vertices = mesh->first_node("vertices");//parse vertices
						if (vertices && vertices->first_attribute("id")->value() == id)
						{
							xml_node<>* vertEntry = vertices->first_node("input");
							while (vertEntry != NULL)
							{
								string inputType = vertEntry->first_attribute("semantic")->value();
								if (inputType == "POSITION")
								{
									vert = GetValues<float>(sources[vertEntry->first_attribute("source")->value()]);
								}
								else if (inputType == "NORMAL")
								{
									normal = GetValues<float>(sources[vertEntry->first_attribute("source")->value()]);
								}
								else if (inputType == "TEXCOORD")
								{
									texcoord = GetValues<float>(sources[vertEntry->first_attribute("source")->value()]);
									
								}
								vertEntry = vertEntry->next_sibling("input");
							}
						}
					}
					else if (type == "NORMAL")
					{
						simple = false;
						normalOffset = atoi(input->first_attribute("offset")->value());
						normal = GetValues<float>(sources[input->first_attribute("source")->value()]);
					}
					else if (type == "TEXCOORD")
					{
						texcoordOffset = atoi(input->first_attribute("offset")->value());
						simple = false;
						texcoord = GetValues<float>(sources[input->first_attribute("source")->value()]);
						texCoordStride = atoi(sources[input->first_attribute("source")->value()]->next_sibling("technique_common")->first_node("accessor")->first_attribute("stride")->value());
					}
					size_t offset = static_cast<size_t>(atoi(input->first_attribute("offset")->value()));
					if (offset > maxOffset) maxOffset = offset;
					input = input->next_sibling("input");
				}
				maxOffset++;
				if (simple)
				{
					//temp
					unsigned int oldSize = loader.vertices.size();
					loader.vertices.resize(oldSize + vert.size() / 3);
					memcpy(&loader.vertices[oldSize], vert.data(), vert.size() * sizeof(float));
					oldSize = loader.normals.size();
					loader.normals.resize(oldSize + normal.size() / 3);
					memcpy(&loader.normals[oldSize], normal.data(), normal.size() * sizeof(float));
					oldSize = loader.textureCoords.size();
					loader.textureCoords.resize(oldSize + texcoord.size() / 2);
					memcpy(&loader.textureCoords[oldSize], texcoord.data(), texcoord.size() * sizeof(float));
					vector<unsigned int>& weightCountPtr = weightCount[meshId];
					loader.weightsCount.insert(loader.weightsCount.end(), weightCountPtr.begin(), weightCountPtr.end());
					vector<unsigned int>& weightIndex = weightIndexes[meshId];
					loader.weightsIndexes.insert(loader.weightsIndexes.end(), weightIndex.begin(), weightIndex.end());
					vector<float>& weightPtr = weights[meshId];
					loader.weights.insert(loader.weights.end(), weightPtr.begin(), weightPtr.end());
				}
				vector<unsigned int> indexes;
				xml_node<>* polygons = triangles->first_node("p");
				while (polygons)
				{
					string value = polygons->value();
					int indexCount = (std::count(value.begin(), value.end(), ' ') + 1) / maxOffset;
					loader.indexes.reserve(loader.indexes.size() + indexCount);
					loader.vertices.reserve(loader.vertices.size() + indexCount);
					loader.normals.reserve(loader.normals.size() + indexCount);
					loader.textureCoords.reserve(loader.textureCoords.size() + indexCount);
					stringstream sstream(value);
					while (!sstream.eof())
					{
						int i;
						sstream >> i;
						if (simple)
						{
							loader.indexes.push_back(i + indexOffset);
						}
						else
						{
							indexes.push_back(i);
							if (indexes.size() == maxOffset)
							{
								CVector3f vertex(&vert[indexes[vertexOffset] * 3]);
								MultiplyVectorToMatrix(vertex, bindShapeMatrices[meshId].data());
								loader.vertices.push_back(vertex);
								loader.normals.push_back(CVector3f(&normal[indexes[normalOffset] * 3]));
								loader.textureCoords.push_back(CVector2f(&texcoord[indexes[texcoordOffset] * texCoordStride]));
								loader.indexes.push_back(loader.vertices.size() - 1);
								if (controllerLib)
								{
									unsigned int count = weightCount[meshId][indexes[vertexOffset]];
									loader.weightsCount.push_back(count);
									unsigned int start = accumulate(weightCount[meshId].begin(), weightCount[meshId].begin() + indexes[vertexOffset], 0);//Get the starting index of the current vertex weight
									loader.weightsIndexes.insert(loader.weightsIndexes.end(), weightIndexes[meshId].begin() + start, weightIndexes[meshId].begin() + start + count);
									loader.weights.insert(loader.weights.end(), weights[meshId].begin() + start, weights[meshId].begin() + start + count);
								}
								indexes.clear();
							}
						}
					}
					polygons = polygons->next_sibling("p");
				}
			}
			mesh = mesh->next_sibling("mesh");
		}
		geometryElement = geometryElement->next_sibling("geometry");
	}
	loader.weightsCount.shrink_to_fit();
	loader.weightsIndexes.shrink_to_fit();
	loader.weights.shrink_to_fit();
	doc.clear();
}