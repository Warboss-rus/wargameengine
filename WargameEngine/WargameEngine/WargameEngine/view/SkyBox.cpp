#include "SkyBox.h"
#include "TextureManager.h"

CSkyBox::CSkyBox(double width, double height, double length, std::wstring const& imageFolder, IRenderer & renderer) 
	: m_height(static_cast<float>(height)), m_width(static_cast<float>(width)), m_length(static_cast<float>(length))
	, m_imageFolder(imageFolder), m_renderer(renderer)
{
}

void CSkyBox::ResetList()
{
	m_list.reset();
}

void CSkyBox::Draw(double x, double y, double z, double scale)
{
	m_renderer.PushMatrix();
	x = -x - m_width / (scale * 2);
	y = -y - m_height / (scale * 2);
	z = -z - m_length / (scale * 2);
	m_renderer.Translate(x, y, z);
	m_renderer.Scale(1.0 / scale);
	if (!m_list)
	{
		m_list = m_renderer.CreateDrawingList([&] {
			static const std::vector<CVector2f> texCoords = { { 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f },{ 0.0f, 1.0f } };
			// Top side
			m_renderer.SetTexture(m_imageFolder + L"/top.bmp", false, TextureFlags::TEXTURE_NO_WRAP);
			m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(m_width, 0.0f, m_length),{ 0.0f, 0.0f, m_length },{ m_width, m_height, m_length },{ 0.0f, m_height, m_length } }, {}, texCoords);
			// Bottom side
			m_renderer.SetTexture(m_imageFolder + L"/bottom.bmp", false, TextureFlags::TEXTURE_NO_WRAP);
			m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(0.0f, 0.0f, 0.0f),{ m_width, 0.0f, 0.0f },{ 0.0f,m_height, 0.0f },{ m_width, m_height, 0.0f } }, {}, texCoords);
			// Front side
			m_renderer.SetTexture(m_imageFolder + L"/front.bmp", false, TextureFlags::TEXTURE_NO_WRAP);
			m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(0.0f, 0.0f, 0.0f),{ 0.0f, 0.0f, m_length },{ m_width, 0.0f, 0.0f },{ m_width, 0.0f, m_length } }, {}, texCoords);
			// Back side
			m_renderer.SetTexture(m_imageFolder + L"/back.bmp", false, TextureFlags::TEXTURE_NO_WRAP);
			m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(m_width, m_height, 0.0f),{ m_width, m_height, m_length },{ 0.0f, m_height, 0.0f },{ 0.0f, m_height, m_length } }, {}, texCoords);
			// Left side
			m_renderer.SetTexture(m_imageFolder + L"/left.bmp", false, TextureFlags::TEXTURE_NO_WRAP);
			m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(0.0f, m_height, 0.0f),{ 0.0f, m_height, m_length },{ 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, m_length } }, {}, texCoords);
			// Right side
			m_renderer.SetTexture(m_imageFolder + L"/right.bmp", false, TextureFlags::TEXTURE_NO_WRAP);
			m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(m_width, 0.0f, 0.0f),{ m_width, 0.0f, m_length },{ m_width, m_height, 0.0f },{ m_width, m_height, m_length } }, {}, texCoords);
			m_renderer.SetTexture(L"");
		});
	}
	m_list->Draw();
	m_renderer.PopMatrix();
}

CSkyBox::~CSkyBox()
{
}