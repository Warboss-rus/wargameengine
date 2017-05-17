#pragma once
#include "../Typedefs.h"
#include "../view/Vector3.h"
#include <memory>
#include <vector>

namespace wargameEngine
{
namespace model
{

struct Bounding
{
	struct Box
	{
		CVector3f min;
		CVector3f max;
	};

	struct Compound
	{
		std::vector<Bounding> items;
	};

	Bounding()
		: type(eType::None)
	{
	}

	Bounding(Box const& box)
		: type(eType::Box)
		, shape(std::make_shared<Box>(box))
	{
	}

	Bounding(Compound const& box)
		: type(eType::Compound)
		, shape(std::make_shared<Compound>(box))
	{
	}

	const Box& GetBox() const
	{
		return *reinterpret_cast<Box*>(shape.get());
	}

	const Compound& GetCompound() const
	{
		return *reinterpret_cast<Compound*>(shape.get());
	}

	float scale = 1.0;

	enum class eType
	{
		Box,
		Compound,
		None,
	} type;

	operator bool()
	{
		return type != eType::None;
	}

private:
	std::shared_ptr<void> shape;
};

class IBoundingBoxManager
{
public:
	virtual Bounding GetBounding(const Path& path) = 0;
	virtual float GetModelScale(const Path& path) = 0;
	virtual CVector3f GetModelRotation(const Path& path) = 0;

	virtual ~IBoundingBoxManager() {}
};
}
}