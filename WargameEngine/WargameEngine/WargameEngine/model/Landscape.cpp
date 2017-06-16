#include "Landscape.h"
#include <math.h>

namespace wargameEngine
{
namespace model
{

Landscape::Landscape()
	: m_width(10.0)
	, m_depth(10.0)
	, m_pointsPerWidth(2)
	, m_pointsPerDepth(2)
	, m_stretchTexture(false)
	, m_horizontalTextureScale(5.0)
	, m_verticalTextureScale(5.0)
{
	m_heights.resize(4);
	for (size_t i = 0; i < 4; ++i)
	{
		m_heights[i] = 0.0;
	}
	m_deltaX = 5.0;
	m_deltaY = 5.0;
}

void Landscape::Reset(float width, float depth, const Path& texture, size_t pointsPerWidth, size_t pointsPerDepth)
{
	m_width = width;
	m_depth = depth;
	m_texture = texture;
	m_pointsPerWidth = pointsPerWidth;
	m_pointsPerDepth = pointsPerDepth;
	m_heights.resize(pointsPerWidth * pointsPerDepth, 0.0f);
	m_deltaX = width / pointsPerWidth;
	m_deltaY = depth / pointsPerDepth;
	if (m_onUpdated)
	{
		m_onUpdated();
	}
}

void Landscape::SetHeight(float x, float y, float value)
{
	m_heights[static_cast<size_t>(round((-m_width / 2 + x) * m_pointsPerWidth - m_depth / 2 + y))] = value;
	if (m_onUpdated)
	{
		m_onUpdated();
	}
}

float Landscape::GetHeight(float x, float y) const
{
	size_t index = static_cast<size_t>((m_width / 2.0f + x) / (m_width / m_pointsPerWidth)) + static_cast<size_t>((m_depth / 2.0f + y) / m_depth);
	return m_heights[index];
}

float Landscape::GetWidth() const
{
	return m_width;
}

void Landscape::AddNewDecal(Decal const& decal)
{
	m_decals.push_back(decal);
	if (m_onUpdated)
	{
		m_onUpdated();
	}
}

size_t Landscape::GetNumberOfDecals() const
{
	return m_decals.size();
}

size_t Landscape::GetPointsPerWidth() const
{
	return m_pointsPerWidth;
}

size_t Landscape::GetPointsPerDepth() const
{
	return m_pointsPerDepth;
}

bool Landscape::isCoordsOnTable(double worldX, double worldY) const
{
	return ((worldX < m_width / 2) && (worldX > -m_width / 2)
		&& (worldY < m_depth / 2) && (worldY > -m_depth / 2));
}

float Landscape::GetDepth() const
{
	return m_depth;
}

float Landscape::GetHorizontalTextureScale() const
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

float Landscape::GetVerticalTextureScale() const
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

const Path& Landscape::GetTexture() const
{
	return m_texture;
}

Decal const& Landscape::GetDecal(size_t index) const
{
	return m_decals[index];
}

void Landscape::DoOnUpdated(const std::function<void()>& onUpdated)
{
	m_onUpdated = onUpdated;
}

}
}