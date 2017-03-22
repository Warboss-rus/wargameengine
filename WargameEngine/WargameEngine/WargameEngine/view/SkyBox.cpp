#include "SkyBox.h"
#include "TextureManager.h"

CSkyBox::CSkyBox(float width, float height, float length, std::wstring const& imageFolder, IRenderer & renderer, CTextureManager & texMan)
	: m_height(height), m_width(width), m_length(length), m_renderer(renderer)
{
	m_images[0] = imageFolder + L"/right.bmp";
	m_images[1] = imageFolder + L"/left.bmp";
	m_images[2] = imageFolder + L"/back.bmp";
	m_images[3] = imageFolder + L"/front.bmp";
	m_images[4] = imageFolder + L"/top.bmp";
	m_images[5] = imageFolder + L"/bottom.bmp";
	m_texture = texMan.CreateCubemapTexture(m_images[0], m_images[1], m_images[2], m_images[3], m_images[4], m_images[5], TextureFlags::TEXTURE_NO_WRAP);
}

void CSkyBox::Draw(CVector3f const& pos, float scale)
{
	if (m_shader)
	{
		m_renderer.GetShaderManager().PushProgram(*m_shader);
		const int skyboxVertices = 36;
		if (!m_buffer)
		{
			const float width = m_width / 2;
			const float height = m_height / 2;
			const float length = m_length / 2;
			const CVector3f vertex[] = {
				// Top side
				{ width, -height, length }, { -width, -height, length }, { width, height, length }, { -width, -height, length }, { width, height, length }, { -width, height, length },
				// Bottom side
				{ -width, -height, -length }, { width, -height, -length }, { -width, height, -length }, { width, -height, -length }, { -width, height, -length }, { width, height, -length },
				// Front side
				{ -width, -height, -length }, { -width, -height, length }, { width, -height, -length }, { -width, -height, length }, { width, -height, -length }, { width, -height, length },
				// Back side
				{ width, height, -length }, { width, height, length }, { -width, height, -length }, { width, height, length }, { -width, height, -length }, { -width, height, length },
				// Left side
				{ -width, height, -length }, { -width, height, length }, { -width, -height, -length }, { -width, height, length }, { -width, -height, -length }, { -width, -height, length },
				// Right side
				{ width, -height, -length }, { width, -height, length }, { width, height, -length }, { width, -height, length }, { width, height, -length }, { width, height, length },
			};
			m_buffer = m_renderer.CreateVertexBuffer(vertex->ptr(), nullptr, nullptr, skyboxVertices);
		}
		m_renderer.PushMatrix();
		float x = -pos.x;
		float y = -pos.y;
		float z = -pos.z;
		m_renderer.Translate(x, y, z);
		m_renderer.Scale(1.0f / scale);
		m_renderer.SetTexture(*m_texture);
		m_buffer->Bind();
		m_buffer->DrawAll(skyboxVertices);
		m_buffer->UnBind();
		m_renderer.UnbindTexture();
		m_renderer.PopMatrix();
		m_renderer.GetShaderManager().PopProgram();
	}
	else
	{
		m_renderer.PushMatrix();
		float x = -pos.x - m_width / (scale * 2);
		float y = -pos.y - m_height / (scale * 2);
		float z = -pos.z - m_length / (scale * 2);
		m_renderer.Translate(x, y, z);
		m_renderer.Scale(1.0f / scale);
		static const std::array<CVector2f, 4> texCoords = { CVector2f{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 0.0f },{ 0.0f, 1.0f } };
		// Top side
		m_renderer.SetTexture(m_images[4], false, TextureFlags::TEXTURE_NO_WRAP);
		m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(m_width, 0.0f, m_length),{ 0.0f, 0.0f, m_length },{ m_width, m_height, m_length },{ 0.0f, m_height, m_length } }, {}, texCoords);
		// Bottom side
		m_renderer.SetTexture(m_images[5], false, TextureFlags::TEXTURE_NO_WRAP);
		m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(0.0f, 0.0f, 0.0f),{ m_width, 0.0f, 0.0f },{ 0.0f,m_height, 0.0f },{ m_width, m_height, 0.0f } }, {}, texCoords);
		// Front side
		m_renderer.SetTexture(m_images[3], false, TextureFlags::TEXTURE_NO_WRAP);
		m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(0.0f, 0.0f, 0.0f),{ 0.0f, 0.0f, m_length },{ m_width, 0.0f, 0.0f },{ m_width, 0.0f, m_length } }, {}, texCoords);
		// Back side
		m_renderer.SetTexture(m_images[2], false, TextureFlags::TEXTURE_NO_WRAP);
		m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(m_width, m_height, 0.0f),{ m_width, m_height, m_length },{ 0.0f, m_height, 0.0f },{ 0.0f, m_height, m_length } }, {}, texCoords);
		// Left side
		m_renderer.SetTexture(m_images[1], false, TextureFlags::TEXTURE_NO_WRAP);
		m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(0.0f, m_height, 0.0f),{ 0.0f, m_height, m_length },{ 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, m_length } }, {}, texCoords);
		// Right side
		m_renderer.SetTexture(m_images[0], false, TextureFlags::TEXTURE_NO_WRAP);
		m_renderer.RenderArrays(RenderMode::TRIANGLE_STRIP, { CVector3f(m_width, 0.0f, 0.0f),{ m_width, 0.0f, m_length },{ m_width, m_height, 0.0f },{ m_width, m_height, m_length } }, {}, texCoords);
		m_renderer.SetTexture(L"");
		m_renderer.PopMatrix();
	}
}

void CSkyBox::SetShaders(std::wstring const& vertex, std::wstring const& fragment)
{
	m_shader = m_renderer.GetShaderManager().NewProgram(vertex, fragment);
}