#include "UIScrollBar.h"

void CUIScrollBar::Update(int size, int contentSize, int width, int step)
{
	m_size = size;
	m_contentSize = contentSize;
	m_width = width;
	m_position = 0;
	m_step = step;
}

void CUIScrollBar::Draw() const
{
	if (m_size >= m_contentSize) return;
	int scrollBegin = m_theme->sbar.buttonHeight;
	int scrollEnd = m_size - m_theme->sbar.buttonHeight;
	int scrollSize = static_cast<int>((scrollEnd - scrollBegin) * (m_size / static_cast<float>(m_contentSize)));
	m_renderer->SetColor(128, 128, 128);
	m_renderer->RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector2i(m_width - m_theme->sbar.width, scrollBegin), { m_width, scrollBegin }, { m_width - m_theme->sbar.width, scrollEnd }, { m_width, scrollEnd } }, {});
	
	int intPos = static_cast<int>(m_position);
	m_renderer->SetColor(200, 200, 200);
	m_renderer->RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector2i(m_width - m_theme->sbar.width, scrollBegin + intPos), { m_width, scrollBegin + intPos },
		{ m_width - m_theme->sbar.width, scrollBegin + intPos + scrollSize }, { m_width, scrollBegin + intPos + scrollSize } }, {});
	m_renderer->SetTexture(m_theme->texture);

	const float * themeTexCoords = (m_buttonPressed) ? m_theme->sbar.pressedTexCoord : m_theme->sbar.texCoord;
	std::vector<CVector2f> texCoords = { CVector2f(themeTexCoords),{ themeTexCoords[2], themeTexCoords[1] },{ themeTexCoords[0], themeTexCoords[3] },{ themeTexCoords[2], themeTexCoords[3] } };
	m_renderer->RenderArrays(RenderMode::TRIANGLE_STRIP,
	{ CVector2i(m_width - m_theme->sbar.width, 0), { m_width, 0 }, { m_width - m_theme->sbar.width, m_theme->sbar.buttonHeight }, { m_width, m_theme->sbar.buttonHeight } }, texCoords);
	
	m_renderer->RenderArrays(RenderMode::TRIANGLE_STRIP,
	{ CVector2i(m_width - m_theme->sbar.width, m_size), { m_width, m_size }, { m_width - m_theme->sbar.width, m_size - m_theme->sbar.buttonHeight }, { m_width, m_size - m_theme->sbar.buttonHeight } }, texCoords);

}

bool CUIScrollBar::LeftMouseButtonDown(int x, int y)
{
	if (!IsOnElement(x, y)) return false;
	if (y < m_theme->sbar.buttonHeight || y > m_size - m_theme->sbar.buttonHeight)
	{
		m_buttonPressed = true;
		return false;
	}
	m_mousePos = y;
	m_pressed = true;
	return true;
}

bool CUIScrollBar::LeftMouseButtonUp(int x, int y)
{
	if (!IsOnElement(x, y)) return false;
	int scrollSize = static_cast<int>(m_size * ((float)m_size / (float)m_contentSize));
	if (y < m_theme->sbar.buttonHeight && m_buttonPressed)//Up Button pressed
	{
		m_position -= (float)m_step * (m_size - 2 * m_theme->sbar.buttonHeight) / (float)m_contentSize;
	} 
	else if (y > m_size - m_theme->sbar.buttonHeight && m_buttonPressed)//Down Button pressed
	{
		m_position += (float)m_step * (m_size - 2 * m_theme->sbar.buttonHeight) / (float)m_contentSize;
	}
	else if (y < m_position || y > m_position + scrollSize)//Outside bar click
	{
		m_position = static_cast<float>(y);
	}
	else if(m_pressed) //Dragging
	{
		m_position += y - m_mousePos;
		m_pressed = false;
	}
	else
	{
		return false;
	}
	if(m_position < 0) m_position = 0;
	if(m_position > m_size - scrollSize) m_position = static_cast<float>(m_size - scrollSize);
	m_buttonPressed = false;
	return true;
}

bool CUIScrollBar::IsOnElement(int x, int y) const
{
	if (m_size >= m_contentSize) return false;
	return (x >= m_width - m_theme->sbar.width && x <= m_width && y >= 0.0f && y <= m_size);
}

int CUIScrollBar::GetPosition() const 
{ 
	return static_cast<int>(m_position * m_contentSize / (m_size - 2 * m_theme->sbar.buttonHeight)); 
}