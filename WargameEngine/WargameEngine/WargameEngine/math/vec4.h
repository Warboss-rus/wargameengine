#pragma once

namespace wargameEngine
{
namespace math
{
template<class T>
struct vec4T
{
	T x = 0;
	T y = 0;
	T z = 0;
	T w = 0;
};

using vec4 = vec4T<float>;
using ivec4 = vec4T<int>;
using uivec4 = vec4T<unsigned>;
using dvec4 = vec4T<double>;
}
}