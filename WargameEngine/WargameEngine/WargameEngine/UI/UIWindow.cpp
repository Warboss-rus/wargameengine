#include "UIWindow.h"
#include "../view/TextWriter.h"
#include "../view/GameView.h"
#include "UIText.h"

CUIWindow::CUIWindow(int width, int height, std::wstring const& headerText, IUIElement * parent, IRenderer & renderer)
	:CUIElement(parent->GetWidth() / 2, parent->GetHeight() / 2, height, width, parent, renderer)
	, m_headerText(headerText), m_dragging(false)
{
	m_x = (600 - m_width) / 2;
	m_y = (600 - m_height) / 2;
}

void CUIWindow::Draw() const
{
	if (!m_visible)
		return;
	m_renderer.PushMatrix();
	m_renderer.Translate(GetX(), GetY(), 0);
	if (!m_cache)
	{
		m_cache = move(m_renderer.RenderToTexture([this]() {
			m_renderer.SetColor(0.4f, 0.4f, 1.0f);
			m_renderer.RenderArrays(RenderMode::RECTANGLES,
			{ CVector2i(0, 0),{ 0, m_theme->window.headerHeight },{ GetWidth(), m_theme->window.headerHeight },{ GetWidth(), 0 } }, {});
			m_renderer.SetTexture(m_theme->texture);
			int right = GetWidth() - m_theme->window.buttonSize;
			float * texCoord = m_theme->window.closeButtonTexCoord;
			m_renderer.RenderArrays(RenderMode::RECTANGLES,
			{ CVector2i(right, 0),{ right, m_theme->window.buttonSize },{ GetWidth(), m_theme->window.buttonSize },{ GetWidth(), 0 } }, 
			{ CVector2f(texCoord[0], texCoord[1]), { texCoord[0], texCoord[3] }, { texCoord[2], texCoord[3] }, { texCoord[2], texCoord[1] } });
			m_renderer.SetTexture("");
			m_renderer.SetColor(m_theme->defaultColor);
			m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
			{ CVector2i(0, m_theme->window.headerHeight),{ GetWidth(), m_theme->window.headerHeight },{ 0, GetHeight() },{ GetWidth(), GetHeight() }, }, {});
			PrintText(m_renderer, 0, 0, GetWidth() - m_theme->window.buttonSize, m_theme->window.headerHeight, m_headerText, m_theme->window.headerText);

		}, GetWidth(), GetHeight()));
	}
	m_cache->Bind();
	m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP,
	{ CVector2i(0, 0),{ GetWidth(), 0 },{ 0, GetHeight() },{ GetWidth(), GetHeight() } },
	{ CVector2f(0.0f, 0.0f),{ 1.0f, 0.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f } });
	m_renderer.Translate(0, m_theme->window.headerHeight, 0);
	CUIElement::Draw();
	m_renderer.PopMatrix();
}

bool CUIWindow::LeftMouseButtonDown(int x, int y)
{
	if (!m_visible || !PointIsOnElement(x, y)) return CUIElement::LeftMouseButtonDown(x, y - m_theme->window.headerHeight);
	int relativeY = y - m_theme->window.headerHeight;
	if (relativeY - GetY() < 0)
	{
		m_dragging = true;
		m_prevX = x;
		m_prevY = y;
		m_parent->SetFocus(this);
	}
	else
	{
		CUIElement::LeftMouseButtonDown(x, relativeY);
	}
	return true;
}

bool CUIWindow::LeftMouseButtonUp(int x, int y)
{
	if (!m_visible || !PointIsOnElement(x, y)) return CUIElement::LeftMouseButtonUp(x, y - m_theme->window.headerHeight);
	int relativeY = y - m_theme->window.headerHeight;
	if (relativeY - GetY() < 0)
	{
		if (GetWidth() - (x - GetX()) < m_theme->window.buttonSize)
		{
			m_parent->SetFocus();
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
		Invalidate();
	}
	CUIElement::OnMouseMove(x, y);
}

int CUIWindow::GetHeight() const
{
	return CUIElement::GetWidth() + m_theme->window.headerHeight * m_windowWidth / 600;
}
