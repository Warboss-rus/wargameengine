#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "ObjectStatic.h"

struct sDecal
{
	Path texture;
	float width;
	float depth;
	float rotation;
	float x;
	float y;
};

class CLandscape
{
public:
	CLandscape();
	void Reset(float width, float depth, const Path& texture, size_t pointsPerWidth, size_t pointsPerDepth);
	void SetHeight(float x, float y, float value);
	float GetHeight(float x, float y) const;
	float GetWidth() const;
	float GetDepth() const;
	float GetHorizontalTextureScale() const;
	float GetVerticalTextureScale() const;
	const Path& GetTexture() const;
	size_t GetPointsPerWidth() const;
	size_t GetPointsPerDepth() const;
	bool isCoordsOnTable(double worldX, double worldY) const;
	void AddNewDecal(sDecal const& decal);
	size_t GetNumberOfDecals() const;
	sDecal const& GetDecal(size_t index) const;
	void AddStaticObject(CStaticObject const& object);
	size_t GetStaticObjectCount() const;
	CStaticObject & GetStaticObject(size_t index);
	void DoOnUpdated(std::function<void()> const& onUpdated);
private:
	float m_width;
	float m_depth;
	float m_deltaX;
	float m_deltaY;
	Path m_texture;
	size_t m_pointsPerWidth;
	size_t m_pointsPerDepth;
	std::vector<float> m_heights;
	std::vector<sDecal> m_decals;
	bool m_stretchTexture;
	float m_horizontalTextureScale;
	float m_verticalTextureScale;
	std::vector<CStaticObject> m_staticObjects;
	std::function<void()> m_onUpdated;
};