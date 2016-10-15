#pragma once
#include <string>
#include "IRenderer.h"

class CSkyBox
{
public:
	CSkyBox(float width, float height, float length, std::wstring const& imageFolder, IRenderer & renderer);
	~CSkyBox();
	void Draw(double x, double y, double z, double scale);
	void ResetList();
private:
	float m_height;
	float m_width;
	float m_length;
	std::unique_ptr<IDrawingList> m_list;
	std::wstring m_imageFolder;
	IRenderer & m_renderer;
};