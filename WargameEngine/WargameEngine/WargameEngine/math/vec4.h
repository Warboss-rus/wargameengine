#pragma once

namespace wargameEngine
{
namespace math
{
template<class T>
struct vec4T
{
	union
	{
		T values[4] = {0, 0, 0, 0};
		struct 
		{
			T x;
			T y;
			T z;
			T w;
		};
		struct
		{
			T r;
			T g;
			T b;
			T a;
		};
	};
};

using vec4 = vec4T<float>;
using ivec4 = vec4T<int>;
using uivec4 = vec4T<unsigned>;
using dvec4 = vec4T<double>;
}
}