#pragma once

namespace wargameEngine
{
inline namespace math
{
template<class T>
struct vec4T
{
	T x;
	T y;
	T z;
	T w;
	operator T*() { return &x; }
	operator const T*() { return &x; }
};

using vec4 = vec4T<float>;
using ivec4 = vec4T<int>;
using uivec4 = vec4T<unsigned>;
using dvec4 = vec4T<double>;
}
}