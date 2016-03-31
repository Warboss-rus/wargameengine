#pragma once
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include "ObjectStatic.h"


struct sDecal
{
	std::string texture;
	double width;
	double depth;
	double rotation;
	double x;
	double y;
};

class CLandscape
{
public:
	CLandscape();
	void Reset(double width, double depth, std::string const& texture, unsigned int pointsPerWidth, unsigned int pointsPerDepth);
	void SetHeight(double x, double y, double value);
	void SetHeight(unsigned int index, double value);
	double GetHeight(double x, double y) const;
	double GetHeight(unsigned int index) const;
	double GetWidth() const;
	double GetDepth() const;
	double GetHorizontalTextureScale() const;
	double GetVerticalTextureScale() const;
	std::string const& GetTexture() const;
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
	double m_width;
	double m_depth;
	double m_deltaX;
	double m_deltaY;
	std::string m_texture;
	unsigned int m_pointsPerWidth;
	unsigned int m_pointsPerDepth;
	std::vector<double> m_heights;
	std::vector<sDecal> m_decals;
	bool m_stretchTexture;
	double m_horizontalTextureScale;
	double m_verticalTextureScale;
	std::vector<CStaticObject> m_staticObjects;
	std::function<void()> m_onUpdated;
};