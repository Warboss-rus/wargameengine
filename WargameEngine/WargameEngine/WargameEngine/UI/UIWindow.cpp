#include "UIWindow.h"

CUIWindow::CUIWindow(int width, int height, std::wstring const& headerText, IUIElement * parent, IRenderer & renderer)
	:CUIElement(0, 0, height, width, parent, renderer), m_headerText(headerText), m_dragging(false)
{

}

void CUIWindow::Draw() const
{
	if (!m_visible)
		return;
	m_renderer.PushMatrix();
	m_renderer.Translate(GetX(), GetY(), 0);
	m_renderer.SetColor(0.4f, 0.4f, 1.0f);
	m_renderer.RenderArrays(RenderMode::RECTANGLES,
	{ CVector2i(0, 0),{ 0, m_theme->window.headerHeight },{ GetWidth(), m_theme->window.headerHeight },{ GetWidth(), 0 } }, {});
	m_renderer.SetColor(1.0f, 0.0f, 0.0f);
	int right = GetWidth() - m_theme->window.buttonSize;
	m_renderer.RenderArrays(RenderMode::RECTANGLES,
	{ CVector2i(right, 0),{ right, m_theme->window.buttonSize },{ GetWidth(), m_theme->window.buttonSize },{ GetWidth(), 0 } }, {});
	m_renderer.Translate(0, m_theme->window.headerHeight, 0);
	m_renderer.SetColor(0.6f, 0.6f, 0.6f);
	m_renderer.RenderArrays(RenderMode::RECTANGLES,
	{ CVector2i(0, 0),{ 0, GetHeight() },{ GetWidth(), GetHeight() },{ GetWidth(), 0 } }, {});
	m_renderer.SetColor(0.0f, 0.0f, 0.0f);
	CUIElement::Draw();
	m_renderer.PopMatrix();
}

bool CUIWindow::LeftMouseButtonDown(int x, int y)
{
	if (!m_visible || !PointIsOnElement(x, y)) return false;
	int relativeY = y - m_theme->window.headerHeight;
	if (relativeY < 0)
	{
		m_dragging = true;
		m_prevX = x;
		m_prevY = y;
	}
	else
	{
		CUIElement::LeftMouseButtonDown(x, relativeY);
	}
	return true;
}

bool CUIWindow::LeftMouseButtonUp(int x, int y)
{
	if (!m_visible || !PointIsOnElement(x, y)) return false;
	int relativeY = y - m_theme->window.headerHeight;
	if (relativeY < 0)
	{
		if (GetWidth() - x < m_theme->window.buttonSize)
		{
			m_parent->DeleteChild(this);//may cause problems
			return true;
		}
	}
	else
	{
		CUIElement::LeftMouseButtonUp(x, relativeY);
	}
	m_dragging = false;
	return true;
}

void CUIWindow::OnMouseMove(int x, int y)
{
	if (m_dragging)
	{
		m_x += x - m_prevX;
		m_y += y - m_prevY;
		m_prevX = x;
		m_prevY = y;
	}
}
