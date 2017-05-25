#include "UIScrollBar.h"
#include "../view/IRenderer.h"

namespace wargameEngine
{
namespace ui
{
UIScrollBar::UIScrollBar(std::shared_ptr<UITheme> theme)
	: m_position(0.0f)
	, m_mousePos(0)
	, m_pressed(false)
	, m_upButtonPressed(false)
	, m_downButtonPressed(false)
	, m_theme(theme)
{
	Update(0, 0, 0, 0);
}

void UIScrollBar::Update(int size, int contentSize, int width, int step)
{
	m_size = size;
	m_contentSize = contentSize;
	m_width = width;
	m_position = 0.0f;
	m_step = step;
}

void UIScrollBar::Draw(view::IRenderer& renderer) const
{
	if (m_size >= m_contentSize)
		return;
	auto& theme = m_theme->sbar;
	int scrollBegin = static_cast<int>(theme.buttonSize * m_scale);
	int scrollEnd = m_size - static_cast<int>(theme.buttonSize * m_scale);
	int scrollSize = static_cast<int>((scrollEnd - scrollBegin) * (m_size / static_cast<float>(m_contentSize)));
	int intPos = static_cast<int>(m_position);
	int scrollWidth = static_cast<int>(theme.width * m_scale);
	int buttonSize = scrollBegin;
	renderer.UnbindTexture();
	renderer.SetColor(theme.backgroundColor);
	renderer.RenderArrays(view::IRenderer::RenderMode::TriangleStrip, { CVector2i(m_width - scrollWidth, scrollBegin), { m_width, scrollBegin }, { m_width - scrollWidth, scrollEnd }, { m_width, scrollEnd } }, {});
	renderer.SetColor(theme.barColor);
	renderer.RenderArrays(view::IRenderer::RenderMode::TriangleStrip, { CVector2i(m_width - scrollWidth, scrollBegin + intPos), { m_width, scrollBegin + intPos }, { m_width - scrollWidth, scrollBegin + intPos + scrollSize }, { m_width, scrollBegin + intPos + scrollSize } }, {});
	renderer.SetColor(0, 0, 0);
	renderer.SetTexture(m_theme->texture, true);
	float* themeTexCoords = (m_upButtonPressed) ? theme.pressedTexCoord : theme.texCoord;
	std::vector<CVector2f> texCoords = { CVector2f(themeTexCoords), { themeTexCoords[2], themeTexCoords[1] }, { themeTexCoords[0], themeTexCoords[3] }, { themeTexCoords[2], themeTexCoords[3] } };
	renderer.RenderArrays(view::IRenderer::RenderMode::TriangleStrip,
		{ CVector2i(m_width - scrollWidth, 0), { m_width, 0 }, { m_width - scrollWidth, scrollBegin }, { m_width, scrollBegin } }, texCoords);

	themeTexCoords = (m_downButtonPressed) ? theme.pressedTexCoord : theme.texCoord;
	texCoords = { CVector2f(themeTexCoords), { themeTexCoords[2], themeTexCoords[1] }, { themeTexCoords[0], themeTexCoords[3] }, { themeTexCoords[2], themeTexCoords[3] } };
	renderer.RenderArrays(view::IRenderer::RenderMode::TriangleStrip,
		{ CVector2i(m_width - scrollWidth, m_size), { m_width, m_size }, { m_width - scrollWidth, m_size - buttonSize }, { m_width, m_size - buttonSize } }, texCoords);
}

bool UIScrollBar::LeftMouseButtonDown(int x, int y)
{
	if (!IsOnElement(x, y))
		return false;
	int buttonSize = static_cast<int>(m_theme->sbar.buttonSize * m_scale);
	if (y < buttonSize)
	{
		m_upButtonPressed = true;
		return false;
	}
	if (y > m_size - buttonSize)
	{
		m_downButtonPressed = true;
		return false;
	}
	m_mousePos = y;
	m_pressed = true;
	return true;
}

bool UIScrollBar::LeftMouseButtonUp(int x, int y)
{
	m_upButtonPressed = false;
	m_downButtonPressed = false;
	m_pressed = false;
	int buttonSize = static_cast<int>(m_theme->sbar.buttonSize);
	if (!IsOnElement(x, y))
		return false;
	int scrollSize = static_cast<int>(m_size * ((float)m_size / (float)m_contentSize));
	if (y < buttonSize && m_upButtonPressed) //Up Button pressed
	{
		m_position -= (float)m_step * (m_size - 2 * buttonSize) / (float)m_contentSize;
	}
	else if (y > m_size - buttonSize && m_downButtonPressed) //Down Button pressed
	{
		m_position += (float)m_step * (m_size - 2 * buttonSize) / (float)m_contentSize;
	}
	else if (y < m_position || y > m_position + scrollSize) //Outside bar click
	{
		m_position = static_cast<float>(y - buttonSize - scrollSize / 2);
	}
	else if (m_pressed) //Dragging
	{
		m_position += y - m_mousePos;
	}
	else
	{
		return false;
	}
	if (m_position < 0)
		m_position = 0;
	if (m_position > m_size - scrollSize)
		m_position = static_cast<float>(m_size - scrollSize);
	return true;
}

bool UIScrollBar::OnMouseMove(int, int y)
{
	if (m_pressed)
	{
		m_position += y - m_mousePos;
		if (m_position < 0)
			m_position = 0;
		int scrollSize = static_cast<int>(m_size * ((float)m_size / (float)m_contentSize));
		if (m_position > m_size - scrollSize)
			m_position = static_cast<float>(m_size - scrollSize);
		m_mousePos = y;
	}
	return m_pressed;
}

bool UIScrollBar::IsOnElement(int x, int y) const
{
	if (m_size >= m_contentSize)
		return false;
	return (x >= m_width - static_cast<int>(m_theme->sbar.width * m_scale) && x <= m_width && y >= 0.0f && y <= m_size);
}

int UIScrollBar::GetPosition() const
{
	return static_cast<int>(m_position * m_contentSize / (m_size - 2 * m_theme->sbar.buttonSize * m_scale));
}

void UIScrollBar::SetScale(float scale)
{
	m_scale = scale;
}
}
}