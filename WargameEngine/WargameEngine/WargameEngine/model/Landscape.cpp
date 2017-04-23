#include "Landscape.h"

CLandscape::CLandscape()
	:m_width(10.0), m_depth(10.0), m_pointsPerWidth(2), m_pointsPerDepth(2), m_stretchTexture(false), m_horizontalTextureScale(2.0), m_verticalTextureScale(2.0)
{
	m_heights.resize(4);
	for (size_t i = 0; i < 4; ++i)
	{
		m_heights[i] = 0.0;
	}
	m_deltaX = 5.0;
	m_deltaY = 5.0;
}

void CLandscape::Reset(float width, float depth, const Path& texture, size_t pointsPerWidth, size_t pointsPerDepth)
{
	m_width = width;
	m_depth = depth;
	m_texture = texture;
	m_pointsPerWidth = pointsPerWidth;
	m_pointsPerDepth = pointsPerDepth;
	m_heights.resize(pointsPerWidth * pointsPerDepth, 0.0f);
	m_deltaX = width / pointsPerWidth;
	m_deltaY = depth / pointsPerDepth;
	if (m_onUpdated) m_onUpdated();
}

void CLandscape::SetHeight(float x, float y, float value)
{
	m_heights[static_cast<size_t>(round((-m_width / 2 + x) * m_pointsPerWidth - m_depth / 2 + y))] = value;
	if (m_onUpdated) m_onUpdated();
}

float CLandscape::GetHeight(float x, float y) const
{
	size_t index = static_cast<size_t>((m_width / 2.0f + x) / (m_width / m_pointsPerWidth)) + static_cast<size_t>((m_depth / 2.0f + y) / m_depth);
	return m_heights[index];
}

float CLandscape::GetWidth() const
{
	return m_width;
}

void CLandscape::AddNewDecal(sDecal const& decal)
{
	m_decals.push_back(decal);
	if (m_onUpdated) m_onUpdated();
}

size_t CLandscape::GetNumberOfDecals() const
{
	return m_decals.size();
}

size_t CLandscape::GetPointsPerWidth() const
{
	return m_pointsPerWidth;
}

size_t CLandscape::GetPointsPerDepth() const
{
	return m_pointsPerDepth;
}

bool CLandscape::isCoordsOnTable(double worldX, double worldY) const
{
	return (worldX < m_width / 2 && worldX > -m_width / 2
		&& worldY < m_depth / 2 && worldY > -m_depth / 2);
}

float CLandscape::GetDepth() const
{
	return m_depth;
}

float CLandscape::GetHorizontalTextureScale() const
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

float CLandscape::GetVerticalTextureScale() const
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

const Path& CLandscape::GetTexture() const
{
	return m_texture;
}

sDecal const& CLandscape::GetDecal(size_t index) const
{
	return m_decals[index];
}

void CLandscape::AddStaticObject(CStaticObject const& object)
{ 
	m_staticObjects.push_back(object); 
	if (m_onUpdated) m_onUpdated();
}

size_t CLandscape::GetStaticObjectCount() const
{
	return m_staticObjects.size();
}

CStaticObject & CLandscape::GetStaticObject(size_t index)
{
	return m_staticObjects[index];
}

void CLandscape::DoOnUpdated(std::function<void()> const& onUpdated)
{
	m_onUpdated = onUpdated;
}
