#pragma once

namespace wargameEngine
{
namespace math
{
template<class T>
struct vec3T
{
	union
	{
		T values[3] = {0, 0, 0};
		struct
		{
			T x;
			T y;
			T z;
		};
		struct
		{
			T r;
			T g;
			T b;
		};
	};
};

using vec3 = vec3T<float>;
using ivec3 = vec3T<int>;
using uivec3 = vec3T<unsigned>;
using dvec3 = vec3T<double>;
}
}