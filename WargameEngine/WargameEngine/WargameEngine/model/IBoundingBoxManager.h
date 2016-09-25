#pragma once
#include <memory>
#include <string>
#include <vector>
#include "..\view\Vector3.h"

struct sBounding
{
	struct sBox
	{
		CVector3d min;
		CVector3d max;
	};
	struct sCompound
	{
		std::vector<sBounding> items;
	};
	sBounding(sBox const& box)
		:type(eType::BOX), shape(std::make_shared<sBox>(box))
	{
	}
	sBounding(sCompound const& box)
		:type(eType::COMPOUND), shape(std::make_shared<sCompound>(box))
	{
	}
	const sBox& GetBox() const
	{
		return *reinterpret_cast<sBox*>(shape.get());
	}
	const sCompound& GetCompound() const
	{
		return *reinterpret_cast<sCompound*>(shape.get());
	}
	double scale = 1.0;
	enum class eType
	{
		BOX,
		COMPOUND,
	} type;
private:
	std::shared_ptr<void> shape;
};

class IBoundingBoxManager
{
public:
	virtual void AddBounding(std::wstring const& path, sBounding const& bounding) = 0;
	virtual sBounding GetBounding(std::wstring const& path) const = 0;

	virtual ~IBoundingBoxManager() {}
};