#pragma once
#include "..\Typedefs.h"
#include "UITheme.h"
#include <functional>

namespace wargameEngine
{
namespace ui
{
template<class T>
struct RectT
{
	T left;
	T top;
	T right;
	T bottom;

	RectT(const T* data)
		: left(data[0]), top(data[1]), right(data[2]), bottom(data[3])
	{}
	RectT(T left, T top, T right, T bottom)
		: left(left), top(top), right(right), bottom(bottom)
	{}
};

struct Point
{
	int x;
	int y;
};

class ICachedTexture
{
public:
	virtual ~ICachedTexture() = default;
};

using RectI = RectT<int>;
using RectF = RectT<float>;

class IUITextHelper
{
public:
	virtual ~IUITextHelper() = default;

	virtual int GetStringWidth(const std::wstring& text, const std::string& font, unsigned fontSize) = 0;
	virtual int GetStringHeight(const std::wstring& text, const std::string& font, unsigned fontSize) = 0;
};

class IUIRenderer : public IUITextHelper
{
public:
	using CachedTexture = std::unique_ptr<ICachedTexture>;

	virtual ~IUIRenderer() = default;

	virtual void Translate(int x, int y) = 0;
	virtual void DrawTexturedRect(const RectI& rect, const RectF& texCoords, const Path& texture) = 0;
	virtual void DrawRect(const RectI& rect, const float* color) = 0;
	virtual void DrawLine(Point a, Point b, const float* color) = 0;
	virtual void DrawText(const RectI& rect, std::wstring const& str, UITheme::Text const& theme, float scale) = 0;
	virtual CachedTexture CreateTexture(int width, int height) = 0;
	virtual void RenderToTexture(CachedTexture& texture, int width, int height, const std::function<void()>& handler) = 0;
	virtual void DrawCachedTexture(const CachedTexture& texture, int width, int height) = 0;
};

class ScopedTranslation
{
public:
	ScopedTranslation(IUIRenderer& renderer, int x, int y)
		: m_renderer(renderer), m_x(x), m_y(y)
	{
		m_renderer.Translate(x, y);
	}
	~ScopedTranslation()
	{
		m_renderer.Translate(-m_x, -m_y);
	}
private:
	IUIRenderer& m_renderer;
	int m_x;
	int m_y;
};
}
}