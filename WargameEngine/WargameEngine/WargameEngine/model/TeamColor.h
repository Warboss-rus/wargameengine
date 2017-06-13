#pragma once
#include <string>
#include <string.h>

namespace wargameEngine
{
namespace model
{
struct TeamColor
{
	std::wstring suffix;
	unsigned char color[3];
};

inline bool operator< (TeamColor const& one, TeamColor const& two) 
{ 
	return one.suffix < two.suffix || memcmp(one.color, two.color, 3) < 0; 
}

}
}