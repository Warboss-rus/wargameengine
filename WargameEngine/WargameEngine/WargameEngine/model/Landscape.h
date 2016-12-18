#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "ObjectStatic.h"


struct sDecal
{
	std::wstring texture;
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
	void Reset(float width, float depth, std::wstring const& texture, unsigned int pointsPerWidth, unsigned int pointsPerDepth);
	void SetHeight(float x, float y, float value);
	void SetHeight(unsigned int index, float value);
	float GetHeight(float x, float y) const;
	float GetHeight(unsigned int index) const;
	float GetWidth() const;
	float GetDepth() const;
	float GetHorizontalTextureScale() const;
	float GetVerticalTextureScale() const;
	std::wstring const& GetTexture() const;
	unsigned int GetPointsPerWidth() const;
	unsigned int GetPointsPerDepth() const;
	bool isCoordsOnTable(double worldX, double worldY) const;
	void AddNewDecal(sDecal const& decal);
	size_t GetNumberOfDecals() const;
	sDecal const& GetDecal(size_t index) const;
	void AddStaticObject(CStaticObject const& object);
	size_t GetStaticObjectCount() const;
	CStaticObject const& GetStaticObject(size_t index) const;
	void DoOnUpdated(std::function<void()> const& onUpdated);
private:
	float m_width;
	float m_depth;
	float m_deltaX;
	float m_deltaY;
	std::wstring m_texture;
	unsigned int m_pointsPerWidth;
	unsigned int m_pointsPerDepth;
	std::vector<float> m_heights;
	std::vector<sDecal> m_decals;
	bool m_stretchTexture;
	float m_horizontalTextureScale;
	float m_verticalTextureScale;
	std::vector<CStaticObject> m_staticObjects;
	std::function<void()> m_onUpdated;
};