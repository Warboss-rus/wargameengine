#pragma once

namespace wargameEngine
{
inline namespace math
{
template<class T>
struct vec2T
{
	T x;
	T y;
	operator T*() { return &x; }
	operator const T*() { return &x; }
};

using vec2 = vec2T<float>;
using ivec2 = vec2T<int>;
using uivec2 = vec2T<unsigned>;
using dvec2 = vec2T<double>;
}
}