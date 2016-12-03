#pragma once
#include <string>
#include "IRenderer.h"
#include "Vector3.h"

class CSkyBox
{
public:
	CSkyBox(float width, float height, float length, std::wstring const& imageFolder, IRenderer & renderer);
	~CSkyBox();
	void Draw(CVector3f const& pos, float scale);
	void ResetList();
private:
	float m_height;
	float m_width;
	float m_length;
	std::unique_ptr<IDrawingList> m_list;
	std::wstring m_imageFolder;
	IRenderer & m_renderer;
};