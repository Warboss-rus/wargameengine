#pragma once
#include <memory>
#include "../Typedefs.h"
#include <vector>
#include "../view/Vector3.h"

struct sBounding
{
	struct sBox
	{
		CVector3f min;
		CVector3f max;
	};
	struct sCompound
	{
		std::vector<sBounding> items;
	};
	sBounding()
		:type(eType::NONE)
	{
	}
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
	float scale = 1.0;
	enum class eType
	{
		BOX,
		COMPOUND,
		NONE,
	} type;
	operator bool() { return type != eType::NONE; }
private:
	std::shared_ptr<void> shape;
};

class IBoundingBoxManager
{
public:
	virtual void AddBounding(const Path& path, sBounding const& bounding) = 0;
	virtual sBounding GetBounding(const Path& path) const = 0;

	virtual ~IBoundingBoxManager() {}
};