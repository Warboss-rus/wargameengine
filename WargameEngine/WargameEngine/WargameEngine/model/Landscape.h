#pragma once
#include "BaseObject.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

struct Decal
{
	Path texture;
	float width;
	float depth;
	float rotation;
	float x;
	float y;
};

class Landscape
{
public:
	Landscape();
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
	void AddNewDecal(const Decal& decal);
	size_t GetNumberOfDecals() const;
	const Decal& GetDecal(size_t index) const;
	void AddStaticObject(const StaticObject& object);
	size_t GetStaticObjectCount() const;
	StaticObject& GetStaticObject(size_t index);
	void DoOnUpdated(const std::function<void()>& onUpdated);

private:
	float m_width;
	float m_depth;
	float m_deltaX;
	float m_deltaY;
	Path m_texture;
	size_t m_pointsPerWidth;
	size_t m_pointsPerDepth;
	std::vector<float> m_heights;
	std::vector<Decal> m_decals;
	bool m_stretchTexture;
	float m_horizontalTextureScale;
	float m_verticalTextureScale;
	std::vector<StaticObject> m_staticObjects;
	std::function<void()> m_onUpdated;
};