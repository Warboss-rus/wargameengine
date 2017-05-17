#include "BoundingBoxManager.h"
#include "../AsyncFileProvider.h"
#include "../Utils.h"
#include <fstream>

namespace wargameEngine
{
namespace model
{

BoundingBoxManager::BoundingBoxManager(AsyncFileProvider& fileProvider)
	:m_fileProvider(fileProvider)
{

}

Bounding BoundingBoxManager::GetBounding(const Path& path)
{
	return GetModelInfo(path).bounding;
}

float BoundingBoxManager::GetModelScale(const Path& path)
{
	return GetModelInfo(path).scale;
}

CVector3f BoundingBoxManager::GetModelRotation(const Path& path)
{
	return GetModelInfo(path).rotation;
}

void BoundingBoxManager::LoadBoundingFromFile(const Path& path)
{
	Path absolutePath = m_fileProvider.GetModelAbsolutePath(path);
	Path boundingPath = absolutePath.substr(0, absolutePath.find_last_of('.')) + make_path(L".txt");
	std::ifstream iFile(boundingPath);
	Bounding::Compound compound;
	float scale = 1.0f;
	CVector3f rotation;
	std::string line;
	while (iFile.good())
	{
		iFile >> line;
		if (line == "box")
		{
			CVector3f min, max;
			iFile >> min.x >> min.y >> min.z >> max.x >> max.y >> max.z;
			Bounding::Box box{ min, max };
			compound.items.push_back(Bounding(box));
		}
		if (line == "scale")
		{
			iFile >> scale;
		}
		if (line == "rotationX")
		{
			iFile >> rotation.x;
		}
		if (line == "rotationY")
		{
			iFile >> rotation.y;
		}
		if (line == "rotationZ")
		{
			iFile >> rotation.z;
		}
	}
	iFile.close();
	Bounding bounding = compound.items.size() == 1 ? compound.items[0] : Bounding(compound);
	m_boundings.emplace(std::make_pair(path, ModelInfo{ bounding, scale, rotation }));
}

BoundingBoxManager::ModelInfo BoundingBoxManager::GetModelInfo(const Path& path)
{
	auto it = m_boundings.find(path);
	if (it == m_boundings.end())
	{
		LoadBoundingFromFile(path);
		it = m_boundings.find(path);
	}
	return it->second;
}

}
}