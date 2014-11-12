#include "ParticleModel.h"
#include <fstream>
#include <map>
#include <string>
#include "../tinyxml.h"
#include "gl.h"
#include "TextureManager.h"

std::vector<float> GetFloatsArray(TiXmlElement* data)
{
	std::vector<float> res;
	char * fl = strtok((char*)data->GetText(), " \n\t");
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
	return std::move(res);
}

float StrToFloat(const char* str, float default)
{
	if (str == NULL) return default;
	return atof(str);
}

CParticleModel::CParticleModel(std::string const& file)
{
	TiXmlDocument doc;
	doc.LoadFile(file);
	TiXmlElement* root = doc.RootElement();
	if (!root) return;
	m_duration = atof(root->Attribute("duration"));
	TiXmlElement* materials = root->FirstChildElement("materials");
	if (!materials) return;
	TiXmlElement* material = materials->FirstChildElement("material");
	std::map<std::string, unsigned int> materialIds;
	while (material)
	{
		materialIds[material->Attribute("id")] =  m_textures.size();
		m_textures.push_back(material->Attribute("texture"));
		CShaderManager shaderman;
		if (material->Attribute("shader")) shaderman.NewProgram(material->Attribute("vertex_shader"), material->Attribute("fragment_shader"));
		m_shaders.push_back(shaderman);
		material = material->NextSiblingElement("material");
	}
	std::map<std::string, unsigned int> particleIDs;
	TiXmlElement* particles = root->FirstChildElement("particles");
	if (!particles) return;
	TiXmlElement* particle = particles->FirstChildElement("particle");
	while (particle)
	{
		std::vector<float> keyframes = GetFloatsArray(particle->FirstChildElement("keyframes"));
		std::vector<float> positions = GetFloatsArray(particle->FirstChildElement("positions"));
		particleIDs[particle->Attribute("id")] = m_particles.size();
		m_particles.push_back(CParticle(keyframes, positions, materialIds[particle->Attribute("material")], atof(particle->Attribute("width")), atof(particle->Attribute("height"))));
		particle = particle->NextSiblingElement("particle");
	}
	TiXmlElement* instances = root->FirstChildElement("instances");
	if (!instances) return;
	TiXmlElement* instance = instances->FirstChildElement("instance");
	while (instance)
	{
		sParticleInstance pinstance;
		pinstance.position.x = StrToFloat(instance->Attribute("x"), 0.0f);
		pinstance.position.y = StrToFloat(instance->Attribute("y"), 0.0f);
		pinstance.position.z = StrToFloat(instance->Attribute("z"), 0.0f);
		pinstance.rotation = StrToFloat(instance->Attribute("rotation"), 0.0f);
		pinstance.speed = StrToFloat(instance->Attribute("speed"), 1.0f);
		pinstance.scale = StrToFloat(instance->Attribute("scale"), 1.0f);
		pinstance.start = StrToFloat(instance->Attribute("start"), 0.0f);
		pinstance.particle = particleIDs[instance->Attribute("particle")];
		m_instances.push_back(std::move(pinstance));
		instance = instance->NextSiblingElement("instance");
	}
	doc.Clear();
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
	for (unsigned int i = 0; i < 3; ++i)
	{
		result[i] = v1[i] * t + v2[i] * (1.0f - t);
	}
	return CVector3f(result);
}

void CParticleModel::Draw(float time) const 
{
	time = fmod(time, GetDuration());
	for (unsigned int i = 0; i < m_instances.size(); ++i)
	{
		CParticle const& particle = m_particles[m_instances[i].particle];
		float partTime = (time - m_instances[i].start) / m_instances[i].speed;
		if (partTime >= 0.0f && partTime <= particle.GetKeyFrames().back())//we need to draw this particle
		{
			glPushMatrix();
			CVector3d const & coords = m_instances[i].position;
			glTranslated(coords.x, coords.y, coords.z);
			glRotated(m_instances[i].rotation, 0.0, 0.0, 1.0);
			glScaled(m_instances[i].scale, m_instances[i].scale, m_instances[i].scale);
			//calculate the position to draw
			std::vector<float> const& keyframes = particle.GetKeyFrames();
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
			DrawParticle(position, particle.GetWidth(), particle.GetHeight());
			m_shaders[particle.GetMaterial()].UnBindProgram();
			CTextureManager::GetInstance()->SetTexture("");
			glPopMatrix();
		}
	}
}

float CParticleModel::GetDuration() const {
	return m_duration;
}