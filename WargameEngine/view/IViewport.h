#pragma once
#include <memory>
#include <functional>

namespace wargameEngine
{
namespace view
{
class ICachedTexture;

class IViewport
{
public:
	virtual ~IViewport() {}

	virtual const ICachedTexture & GetTexture(size_t index) const = 0;

	virtual const float* GetProjectionMatrix() const = 0;
	virtual const float* GetViewMatrix() const = 0;

	virtual int GetX() const = 0;
	virtual int GetY() const = 0;
	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
};
}
}