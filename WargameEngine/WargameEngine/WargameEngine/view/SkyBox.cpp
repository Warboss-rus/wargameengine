#include "SkyBox.h"
#include "TextureManager.h"

CSkyBox::CSkyBox(double width, double height, double length, std::string const& imageFolder, IRenderer & renderer) 
	: m_width(static_cast<float>(width)), m_height(static_cast<float>(height)), m_length(static_cast<float>(length))
	, m_imageFolder(imageFolder), m_renderer(renderer)
{
	ResetList();
}

void CSkyBox::ResetList()
{
	m_list = m_renderer.CreateDrawingList([&] {
		static const std::vector<CVector2f> texCoords = { { 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f },{ 0.0f, 1.0f } };
		// Top side
		m_renderer.SetTexture(m_imageFolder + "/top.bmp", false, CTextureManager::TEXTURE_NO_WRAP);
		m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(m_width, 0.0f, m_length), { 0.0f, 0.0f, m_length },{ m_width, m_height, m_length }, {0.0f, m_height, m_length }}, {}, texCoords);
		// Bottom side
		m_renderer.SetTexture(m_imageFolder + "/bottom.bmp", false, CTextureManager::TEXTURE_NO_WRAP);
		m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(0.0f, 0.0f, 0.0f),{ m_width, 0.0f, 0.0f },{ 0.0f,m_height, 0.0f },{ m_width, m_height, 0.0f } }, {}, texCoords);
		// Front side
		m_renderer.SetTexture(m_imageFolder + "/front.bmp", false, CTextureManager::TEXTURE_NO_WRAP);
		m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(0.0f, 0.0f, 0.0f),{ 0.0f, 0.0f, m_length },{ m_width, 0.0f, 0.0f },{ m_width, 0.0f, m_length } }, {}, texCoords);
		// Back side
		m_renderer.SetTexture(m_imageFolder + "/back.bmp", false, CTextureManager::TEXTURE_NO_WRAP);
		m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(m_width, m_height, 0.0f),{ m_width, m_height, m_length },{ 0.0f, m_height, 0.0f },{ 0.0f, m_height, m_length } }, {}, texCoords);
		// Left side
		m_renderer.SetTexture(m_imageFolder + "/left.bmp", false, CTextureManager::TEXTURE_NO_WRAP);
		m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(0.0f, m_height, 0.0f),{ 0.0f, m_height, m_length },{ 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, m_length } }, {}, texCoords);
		// Right side
		m_renderer.SetTexture(m_imageFolder + "/right.bmp", false, CTextureManager::TEXTURE_NO_WRAP);
		m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(m_width, 0.0f, 0.0f),{ m_width, 0.0f, m_length },{ m_width, m_height, 0.0f },{ m_width, m_height, m_length } }, {}, texCoords);
		m_renderer.SetTexture("");
	});
}

void CSkyBox::Draw(double x, double y, double z, double scale)
{
	m_renderer.PushMatrix();
	x = -x - m_width / (scale * 2);
	y = -y - m_height / (scale * 2);
	z = -z - m_length / (scale * 2);
	m_renderer.Translate(x, y, z);
	m_renderer.Scale(1.0 / scale);
	m_list->Draw();
	m_renderer.PopMatrix();
}

CSkyBox::~CSkyBox()
{
}