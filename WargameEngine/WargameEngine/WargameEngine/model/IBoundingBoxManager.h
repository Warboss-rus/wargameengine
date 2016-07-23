#pragma once
#include <memory>
#include <string>
class IBounding;

class IBoundingBoxManager
{
public:
	virtual void AddBoundingBox(std::wstring const& path, std::shared_ptr<IBounding> bbox) = 0;

	virtual ~IBoundingBoxManager() {}
};