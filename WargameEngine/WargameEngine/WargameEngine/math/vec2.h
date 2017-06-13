#pragma once

namespace wargameEngine
{
namespace math
{
template<class T>
struct vec2T
{
	union
	{
		T values[2] = { 0, 0 };
		struct
		{
			T x;
			T y;
		};
	};
};

using vec2 = vec2T<float>;
using ivec2 = vec2T<int>;
using uivec2 = vec2T<unsigned>;
using dvec2 = vec2T<double>;
}
}