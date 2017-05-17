#pragma once
#include "../Typedefs.h"
#include "IRenderer.h"
#include "IShaderManager.h"
#include "Vector3.h"

namespace wargameEngine
{
namespace view
{
class TextureManager;

class SkyBox
{
public:
	SkyBox(float width, float height, float length, const Path& imageFolder, IRenderer& renderer, TextureManager& texMan);
	void Draw(CVector3f const& pos, float scale);
	void SetShaders(const Path& vertex, const Path& fragment);

private:
	float m_height;
	float m_width;
	float m_length;
	Path m_images[6];
	IRenderer& m_renderer;
	std::unique_ptr<ICachedTexture> m_texture;
	std::unique_ptr<IVertexBuffer> m_buffer;
	std::unique_ptr<IShaderProgram> m_shader;
};
}
}