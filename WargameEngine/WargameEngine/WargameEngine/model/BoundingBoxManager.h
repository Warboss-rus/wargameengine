#pragma once
#include "IBoundingBoxManager.h"
#include <unordered_map>

namespace wargameEngine
{
class AsyncFileProvider;

namespace model
{
class BoundingBoxManager : public IBoundingBoxManager
{
public:
	BoundingBoxManager(AsyncFileProvider& fileProvider);

	Bounding GetBounding(const Path& path) override;
	float GetModelScale(const Path& path) override;
	CVector3f GetModelRotation(const Path& path) override;
private:
	struct ModelInfo
	{
		Bounding bounding;
		float scale;
		CVector3f rotation;
	};

	void LoadBoundingFromFile(const Path& path);
	ModelInfo GetModelInfo(const Path& path);

	AsyncFileProvider& m_fileProvider;
	std::unordered_map<Path, ModelInfo> m_boundings;
};
}
}