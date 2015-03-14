#include "Landscape.h"
#include "../view/GameView.h"

CLandscape::CLandscape()
	:m_width(10.0), m_depth(10.0), m_texture(""), m_pointsPerWidth(2), m_pointsPerDepth(2), m_stretchTexture(false), m_horizontalTextureScale(2.0), m_verticalTextureScale(2.0)
{
	m_heights.resize(4);
	for (unsigned int i = 0; i < 4; ++i)
	{
		m_heights[i] = 0.0;
	}
	m_deltaX = 5.0;
	m_deltaY = 5.0;
}

CLandscape::CLandscape(double width, double depth, std::string const& texture, unsigned int pointsPerWidth, unsigned int pointsPerDepth)
	:m_width(width), m_depth(depth), m_texture(texture), m_pointsPerWidth(pointsPerWidth), m_pointsPerDepth(pointsPerDepth), m_stretchTexture(false), m_horizontalTextureScale(2.0), m_verticalTextureScale(2.0)
{
	m_heights.resize(pointsPerWidth * pointsPerDepth);
	for (unsigned int i = 0; i < pointsPerWidth * pointsPerDepth; ++i)
	{
		m_heights[i] = 0.0;
	}
	m_deltaX = width / pointsPerWidth;
	m_deltaY = depth / pointsPerDepth;
	CGameView::GetInstance().lock()->ResetTable();
}

void CLandscape::SetHeight(double x, double y, double value)
{
	m_heights[static_cast<size_t>(round((-m_width / 2 + x) * m_pointsPerWidth - m_depth / 2 + y))] = value;
	CGameView::GetInstance().lock()->ResetTable();
}

double CLandscape::GetHeight(double x, double y) const
{
	return m_heights[static_cast<size_t>((m_width / 2 + x) / (m_width / m_pointsPerWidth) + (m_depth / 2 + y) / (m_depth / m_pointsPerDepth))];
}

void CLandscape::AddNewDecal(sDecal const& decal)
{
	m_decals.push_back(decal);
	CGameView::GetInstance().lock()->ResetTable();
}

bool CLandscape::isCoordsOnTable(double worldX, double worldY) const
{
	return (worldX < m_width / 2 && worldX > -m_width / 2
		&& worldY < m_depth / 2 && worldY > -m_depth / 2);
}

double CLandscape::GetHorizontalTextureScale() const
{
	if (m_stretchTexture)
	{
		return m_horizontalTextureScale * GetWidth();
	}
	else
	{
		return m_horizontalTextureScale;
	}
}

double CLandscape::GetVerticalTextureScale() const
{
	if (m_stretchTexture)
	{
		return m_verticalTextureScale * GetDepth();
	}
	else
	{
		return m_verticalTextureScale;
	}
}

void CLandscape::AddStaticObject(CStaticObject const& object)
{ 
	m_staticObjects.push_back(object); 
	CGameView::GetInstance().lock()->ResetTable();
}