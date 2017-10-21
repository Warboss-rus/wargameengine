#pragma once

namespace wargameEngine
{
inline namespace math
{
template<class T>
struct quatT
{
	T x;
	T y;
	T z;
	T w;
	operator T*() { return &x; }
	operator const T*() { return &x; }
};

using quat = quatT<float>
}
}