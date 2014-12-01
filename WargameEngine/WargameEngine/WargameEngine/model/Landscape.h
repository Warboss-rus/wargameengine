#include <vector>
#include <string>

#pragma once

class CLandscape
{
public:
	CLandscape();
	CLandscape(double width, double depth, std::string const& texture, unsigned int pointsPerWidth, unsigned int pointsPerDepth);
	void SetHeight(double x, double y, double value);
	void SetHeight(unsigned int index, double value) {m_heights[index] = value; }
	double GetHeight(double x, double y) const;
	double GetHeight(unsigned int index) const { return m_heights[index]; }
	double GetWidth() const { return m_width; }
	double GetDepth() const { return m_depth; }
	std::string const& GetTexture() const { return m_texture; }
	unsigned int GetPointsPerWidth() const { return m_pointsPerWidth; }
	unsigned int GetPointsPerDepth() const { return m_pointsPerDepth; }
	bool isCoordsOnTable(double worldX, double worldY) const;
	void AddNewDecal(std::string const& texture, float width, float depth, float rotation);
	unsigned int GetNumberOfDecals() const { return m_decals.size(); }
	std::string const& GetDecalTexture(unsigned int index) { return m_decals[index].texture; }
	float GetDecalWidth(unsigned int index) { return m_decals[index].width; }
	float GetDecalDepth(unsigned int index) { return m_decals[index].depth; }
	float GetDecalRotation(unsigned int index) { return m_decals[index].rotation; }
	double GetHorizontalTextureScale() const;
	double GetVerticalTextureScale() const;
private:
	struct sDecal
	{
		std::string texture;
		float width;
		float depth;
		float rotation;
	};
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
};