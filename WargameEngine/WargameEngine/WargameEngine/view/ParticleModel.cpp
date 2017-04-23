#include "ParticleModel.h"
#include <fstream>
#include <map>
#include <limits.h>
#include <string.h>
#include "../rapidxml/rapidxml.hpp"
#include "../Utils.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

using namespace std;
using namespace rapidxml;

namespace
{
vector<float> SplitFloatsBy(string const& data, char delimeter)
{
	vector<float> result;
	size_t it1 = 0;
	size_t it2 = data.find(delimeter);
	while (it2 != data.npos)
	{
		result.push_back(stof(data.substr(it1, it2)));
		it1 = it2 + 1;
		it2 = data.find(delimeter, it1);
	}
	if (it1 != data.npos)
	{
		result.push_back(stof(data.substr(it1)));
	}
	return result;
}

CVector2f GetVector2f(std::string const& data)
{
	auto arr = SplitFloatsBy(data, ';');
	return{ arr[0], arr[1] };
}

std::pair<float, float> GetPair(std::string const& data)
{
	auto arr = SplitFloatsBy(data, '-');
	return{ arr[0], arr[1] };
}

float frand(float start, float end)
{
	double r = static_cast<double>(rand()) / RAND_MAX;
	return start + static_cast<float>(r * fabs(end - start));
}
float radians(float degrees)
{
	return degrees * (float)M_PI / 180;
}
class CSphereEmitter : public CParticleModel::IEmitter
{
public:
	CSphereEmitter(float minInclination, float maxInclination, float minAzimuth, float maxAzimuth, float minRadius, float maxRadius, float minSpeed, float maxSpeed, float * color)
		:m_minInclination(minInclination), m_maxInclination(maxInclination), m_minAzimuth(minAzimuth), m_maxAzimuth(maxAzimuth)
		, m_minRadius(minRadius), m_maxRadius(maxRadius), m_minSpeed(minSpeed), m_maxSpeed(maxSpeed)
	{
		memcpy(m_color, color, sizeof(float) * 4);
	}
	virtual void InitParticle(sParticle & particle) override
	{
		float inclination = radians(frand(m_minInclination, m_maxInclination));
		float azimuth = radians(frand(m_minAzimuth, m_maxAzimuth));

		float radius = frand(m_minRadius, m_maxRadius);
		float speed = frand(m_minSpeed, m_maxSpeed);

		float sInclination = sinf(inclination);

		float X = sInclination * cosf(azimuth);
		float Y = sInclination * sinf(azimuth);
		float Z = cosf(inclination);

		CVector3f vector(Z, Y, X);

		CVector3f position = vector * radius;
		memcpy(particle.m_position, &position.x, sizeof(float) * 3);
		particle.m_velocity = vector * speed;
		memcpy(particle.m_color, m_color, sizeof(float) * 4);
	}

private:
	float m_minInclination, m_maxInclination;
	float m_minAzimuth, m_maxAzimuth;
	float m_minRadius, m_maxRadius;
	float m_minSpeed, m_maxSpeed;
	float m_color[4];
};
class CFrameUpdater : public CParticleModel::IUpdater
{
public:
	struct sFrame
	{
		float startTime;
		CVector2f texCoords = { -1, -1 };
	};
	CFrameUpdater(std::vector<sFrame> && frames)
		:m_frames(std::move(frames))
	{
		std::sort(m_frames.begin(), m_frames.end(), [](sFrame const& f1, sFrame const& f2) {return f1.startTime < f2.startTime;});
	}

	virtual void Update(std::vector<sParticle> & particles) override
	{
		for (auto& particle : particles)
		{
			float lifePercent = particle.m_age / particle.m_lifeTime;
			size_t begin = 0;
			size_t end = m_frames.size();
			size_t center;
			while (end - begin > 1)
			{
				center = (begin + end) / 2;
				if (lifePercent < m_frames[center].startTime)
				{
					end = center;
				}
				else
				{
					begin = center;
				}
			}
			memcpy(particle.m_texCoord, m_frames[begin].texCoords, sizeof(float) * 2);
		}
	}
private:
	std::vector<sFrame> m_frames;
};
}

CParticleModel::CParticleModel(const Path& file)
{
	ifstream istream(file);
	string content((istreambuf_iterator<char>(istream)), istreambuf_iterator<char>());
	std::unique_ptr<xml_document<>> doc = std::make_unique<xml_document<>>();
	doc->parse<parse_trim_whitespace>(&content[0]);
	xml_node<>* root = doc->first_node();
	if (!root) return;
	xml_node<>* texture = root->first_node("texture");
	if (texture)
	{
		m_texture = make_path(texture->first_attribute("path")->value());
		m_textureFrameSize = GetVector2f(texture->first_attribute("frameSize")->value());
	}
	xml_node<>* particle = root->first_node("particle");
	if (particle)
	{
		m_particleSize = GetVector2f(particle->first_attribute("size")->value());
	}
	xml_node<>* emitter = root->first_node("emitter");
	if (emitter)
	{
		auto p = GetPair(emitter->first_attribute("lifeTime")->value());
		m_minLifeTime = p.first;
		m_maxLifeTime = p.second;
		p = GetPair(emitter->first_attribute("scale")->value());
		m_minScale = p.first;
		m_maxScale = p.second;
		xml_node<>* sphereEmitter = emitter->first_node("sphereEmitter");
		if (sphereEmitter)
		{
			auto inclination = GetPair(sphereEmitter->first_attribute("inclination")->value());
			auto azimuth = GetPair(sphereEmitter->first_attribute("azimuth")->value());
			auto radius = GetPair(sphereEmitter->first_attribute("radius")->value());
			auto speed = GetPair(sphereEmitter->first_attribute("speed")->value());
			auto color = SplitFloatsBy(sphereEmitter->first_attribute("color")->value(), ';');
			m_emitter = std::make_unique<CSphereEmitter>(inclination.first, inclination.second, azimuth.first, azimuth.second, radius.first, radius.second, speed.first, speed.second, color.data());
		}
	}
	xml_node<>* updater = root->first_node("updater");
	if (updater)
	{
		xml_node<>* frameUpdater = updater->first_node("frameUpdater");
		if (frameUpdater)
		{
			std::vector<CFrameUpdater::sFrame> frames;
			xml_node<>* frame = frameUpdater->first_node("frame");
			while (frame)
			{
				CFrameUpdater::sFrame frameStruct;
				frameStruct.startTime = std::stof(frame->first_attribute("start")->value());
				if (frame->first_attribute("texFrameIndex"))
				{
					frameStruct.texCoords = GetVector2f(frame->first_attribute("texFrameIndex")->value()) / m_textureFrameSize;
					frameStruct.texCoords.y = 1.0f - frameStruct.texCoords.y;
				}
				frames.push_back(frameStruct);
				frame = frame->next_sibling("frame");
			}
			m_updater = std::make_unique<CFrameUpdater>(std::move(frames));
		}
	}
	doc->clear();
}

Path CParticleModel::GetTexture() const
{
	return m_texture;
}

CVector2f CParticleModel::GetTextureFrameSize() const
{
	return m_textureFrameSize;
}

CVector2f CParticleModel::GetParticleSize() const
{
	return m_particleSize;
}

bool CParticleModel::HasDifferentTexCoords() const
{
	return true;
}

bool CParticleModel::HasDifferentColors() const
{
	return false;
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

float CParticleModel::GetAverageLifeTime() const
{
	return (m_maxLifeTime + m_minLifeTime) * 0.5f;
}

void CParticleModel::InitParticle(sParticle & particle) const
{
	if (m_emitter)
	{
		m_emitter->InitParticle(particle);
	}
	particle.m_lifeTime = frand(m_minLifeTime, m_maxLifeTime);
	*particle.m_scale = frand(m_minScale, m_maxScale);
	particle.m_texCoord[0] = 0.0f;
	particle.m_texCoord[1] = 0.0f;
}

void CParticleModel::UpdateParticles(std::vector<sParticle> & particles) const
{
	if (m_updater)
	{
		m_updater->Update(particles);
	}
}
