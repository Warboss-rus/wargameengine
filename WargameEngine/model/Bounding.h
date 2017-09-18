#pragma once
#include "..\view\Vector3.h"
#include <vector>
#include <variant>

namespace wargameEngine
{
namespace model
{

struct Bounding
{
	struct Box
	{
		CVector3f min = { -0.5f, -0.5f, -0.5f };
		CVector3f max = { 0.5f, 0.5f, 0.5f };
	};

	struct Compound
	{
		std::vector<Bounding> items;
	};

	using Variant = std::variant<Box, Compound>;

	Variant data;
	float scale = 1.0f;
};
}
}