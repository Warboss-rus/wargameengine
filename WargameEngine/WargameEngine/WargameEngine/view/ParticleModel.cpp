#include "ParticleModel.h"
#include <fstream>
#include <map>
#include <string>
#include "gl.h"
#include "TextureManager.h"
#include <cstring>
#include "..\rapidxml\rapidxml.hpp"
#include <sstream>

using namespace std;
using namespace rapidxml;

vector<float> GetFloatsArray(xml_node<>* data)
{
	vector<float> res;
	stringstream sstream(data->value());
	while (!sstream.eof())
	{
		float val;
		sstream >> val;
		res.push_back(val);
	}
	res.shrink_to_fit();
	return move(res);
}

float StrToFloat(xml_attribute<>* strAttr, float defaultValue)
{
	if (strAttr == NULL) return defaultValue;
	string value = strAttr->value();
	if (value.substr(0, 5) == "rand(")
	{
		double from = atof(value.substr(5, value.find(',') - 5).c_str());
		double to = atof(value.substr(value.find(',') + 1).c_str());
		return static_cast <float> (from + rand()) / (static_cast <float> (RAND_MAX / (to - from)));
	}
	else
	{
		return static_cast<float>(atof(strAttr->value()));
	}
}

CParticleModel::CParticleModel(string const& file)
{
	ifstream istream(file);
	string content((istreambuf_iterator<char>(istream)), istreambuf_iterator<char>());
	xml_document<> doc;
	doc.parse<parse_trim_whitespace>(&content[0]);
	xml_node<>* root = doc.first_node();
	if (!root) return;
	m_duration = static_cast<float>(atof(root->first_attribute("duration")->value()));
	xml_node<>* materials = root->first_node("materials");
	if (!materials) return;
	xml_node<>* material = materials->first_node("material");
	map<string, unsigned int> materialIds;
	while (material)
	{
		materialIds[material->first_attribute("id")->value()] =  m_textures.size();
		m_textures.push_back(material->first_attribute("texture")->value());
		CShaderManager shaderman;
		if (material->first_attribute("shader")) shaderman.NewProgram(material->first_attribute("vertex_shader")->value(), material->first_attribute("fragment_shader")->value());
		m_shaders.push_back(shaderman);
		material = material->next_sibling("material");
	}
	map<string, unsigned int> particleIDs;
	xml_node<>* particles = root->first_node("particles");
	if (!particles) return;
	xml_node<>* particle = particles->first_node("particle");
	while (particle)
	{
		vector<float> keyframes = GetFloatsArray(particle->first_node("keyframes"));
		vector<float> positions = GetFloatsArray(particle->first_node("positions"));
		particleIDs[particle->first_attribute("id")->value()] = m_particles.size();
		float width = static_cast<float>(atof(particle->first_attribute("width")->value()));
		float height = static_cast<float>(atof(particle->first_attribute("height")->value()));
		m_particles.push_back(CParticle(keyframes, positions, materialIds.at(particle->first_attribute("material")->value()), width, height));
		particle = particle->next_sibling("particle");
	}
	xml_node<>* instances = root->first_node("instances");
	if (!instances) return;
	xml_node<>* instance = instances->first_node("instance");
	while (instance)
	{
		sParticleInstance pinstance;
		pinstance.position.x = StrToFloat(instance->first_attribute("x"), 0.0f);
		pinstance.position.y = StrToFloat(instance->first_attribute("y"), 0.0f);
		pinstance.position.z = StrToFloat(instance->first_attribute("z"), 0.0f);
		pinstance.rotation = StrToFloat(instance->first_attribute("rotation"), 0.0f);
		pinstance.speed = StrToFloat(instance->first_attribute("speed"), 1.0f);
		pinstance.scale = StrToFloat(instance->first_attribute("scale"), 1.0f);
		pinstance.start = StrToFloat(instance->first_attribute("start"), 0.0f);
		pinstance.particle = particleIDs[instance->first_attribute("particle")->value()];
		xml_node<>* uniform = instance->first_node("uniform");
		while (uniform)
		{
			pinstance.uniforms[uniform->first_attribute("key")->value()] = GetFloatsArray(uniform);
			uniform = uniform->next_sibling("uniform");
		}
		m_instances.push_back(move(pinstance));
		instance = instance->next_sibling("instance");
	}
	xml_node<> * randomInstance = instances->first_node("random_instance");
	while (randomInstance)
	{
		int count = atoi(randomInstance->first_attribute("count")->value());
		for (int i = 0; i < count; ++i)
		{
			sParticleInstance pinstance;
			pinstance.position.x = StrToFloat(randomInstance->first_attribute("x"), 0.0f);
			pinstance.position.y = StrToFloat(randomInstance->first_attribute("y"), 0.0f);
			pinstance.position.z = StrToFloat(randomInstance->first_attribute("z"), 0.0f);
			pinstance.rotation = StrToFloat(randomInstance->first_attribute("rotation"), 0.0f);
			pinstance.speed = StrToFloat(randomInstance->first_attribute("speed"), 1.0f);
			pinstance.scale = StrToFloat(randomInstance->first_attribute("scale"), 1.0f);
			pinstance.start = StrToFloat(randomInstance->first_attribute("start"), 0.0f);
			pinstance.particle = particleIDs[randomInstance->first_attribute("particle")->value()];
			m_instances.push_back(move(pinstance));
			randomInstance = randomInstance->next_sibling("random_instance");
		}
	}
	doc.clear();
}

void DrawParticle(CVector3f const& position, float width, float height)
{
	float modelview[4][4];
	glGetFloatv(GL_MODELVIEW_MATRIX, &modelview[0][0]);

	float sizeX2 = width * 0.5f;
	float sizeY2 = height * 0.5f;
	// ��������� ����������� ������������ ���� �� ������� �������������-���� 
	// � �������� jcb x � y �� �������� ������� ���������
	CVector3d xAxis(modelview[0][0] * sizeX2, modelview[1][0] * sizeX2, modelview[2][0] * sizeX2);
	CVector3d yAxis(modelview[0][1] * sizeY2, modelview[1][1] * sizeY2, modelview[2][1] * sizeY2);
	CVector3d zAxis(modelview[0][2], modelview[1][2], modelview[2][2]);

	// ������� �������� ������� ������ ��������� ������������ ��� ������
	CVector3d p0(-xAxis.x + yAxis.x + position.x, -xAxis.y + yAxis.y + position.y, -xAxis.z + yAxis.z + position.z);
	CVector3d p1(-xAxis.x - yAxis.x + position.x, -xAxis.y - yAxis.y + position.y, -xAxis.z - yAxis.z + position.z);
	CVector3d p2(+xAxis.x - yAxis.x + position.x, +xAxis.y - yAxis.y + position.y, +xAxis.z - yAxis.z + position.z);
	CVector3d p3(+xAxis.x + yAxis.x + position.x, +xAxis.y + yAxis.y + position.y, +xAxis.z + yAxis.z + position.z);

	glBegin(GL_TRIANGLE_STRIP);
	{
		glTexCoord2d(0.0, 0.0);
		glVertex3d(p0.x, p0.y, p0.z);
		glTexCoord2d(0.0, 1.0);
		glVertex3d(p1.x, p1.y, p1.z);
		glTexCoord2d(1.0, 0.0);
		glVertex3d(p3.x, p3.y, p3.z);
		glTexCoord2d(1.0, 1.0);
		glVertex3d(p2.x, p2.y, p2.z);
	}
	glEnd();
}

CVector3f InterpolateVectors(CVector3f const& v1, CVector3f const& v2, float t)
{
	float result[3];
	for (size_t i = 0; i < 3; ++i)
	{
		result[i] = v1[i] * t + v2[i] * (1.0f - t);
	}
	return CVector3f(result);
}

void CParticleModel::Draw(float time) const 
{
	time = fmod(time, GetDuration());
	for (size_t i = 0; i < m_instances.size(); ++i)
	{
		CParticle const& particle = m_particles[m_instances[i].particle];
		float partTime = (time - m_instances[i].start) / (float)m_instances[i].speed;
		if (partTime >= 0.0f && partTime <= particle.GetKeyFrames().back())//we need to draw this particle
		{
			glPushMatrix();
			CVector3d const & coords = m_instances[i].position;
			glTranslated(coords.x, coords.y, coords.z);
			glRotated(m_instances[i].rotation, 0.0, 1.0, 0.0);
			glScaled(m_instances[i].scale, m_instances[i].scale, m_instances[i].scale);
			//calculate the position to draw
			vector<float> const& keyframes = particle.GetKeyFrames();
			unsigned int j = 0;//a frame to draw
			for (j; j < keyframes.size(); ++j)
			{
				if (partTime <= keyframes[j] && (j == 0 || keyframes[j - 1] <= partTime))
				{
					break;
				}
			}
			//interpolate position here
			if (j == keyframes.size()) j--;
			CVector3f position = particle.GetPositions()[j];
			if (j > 0)
			{
				position = InterpolateVectors(particle.GetPositions()[j], particle.GetPositions()[j - 1], (partTime - keyframes[j - 1]) / (keyframes[j] - keyframes[j - 1]));
			}
			CTextureManager::GetInstance()->SetTexture(m_textures[particle.GetMaterial()]);
			m_shaders[particle.GetMaterial()].BindProgram();
			for (auto k = m_instances[i].uniforms.begin(); k != m_instances[i].uniforms.end(); ++k)
			{
				m_shaders[particle.GetMaterial()].SetUniformValue(k->first, k->second.size(), &k->second[0]);
			}
			DrawParticle(position, particle.GetWidth(), particle.GetHeight());
			for (auto k = m_instances[i].uniforms.begin(); k != m_instances[i].uniforms.end(); ++k)
			{
				m_shaders[particle.GetMaterial()].SetUniformValue(k->first, 0, (float*)nullptr);
			}
			m_shaders[particle.GetMaterial()].UnBindProgram();
			CTextureManager::GetInstance()->SetTexture("");
			glPopMatrix();
		}
	}
}

float CParticleModel::GetDuration() const {
	return m_duration;
}