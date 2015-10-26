#include "DirectXRenderer.h"
#include <Windows.h>
#include "ShaderManagerDirectX.h"

class CDirectXCachedTexture : public ICachedTexture
{
public:
	virtual void Bind() const override
	{
	}

	virtual void UnBind() const override
	{
	}
private:
};

class CDirectXDrawingList : public IDrawingList
{
public:
	virtual void Draw() const override
	{
	}
private:
};

class CDirectXVertexBuffer : public IVertexBuffer
{
public:
	virtual void Bind() const override
	{
	}

	virtual void DrawIndexes(unsigned int * indexPtr, size_t count) override
	{
	}

	virtual void DrawAll(size_t count) override
	{
	}

	virtual void UnBind() const override
	{
	}
private:
};

class CDirectXFrameBuffer : public IFrameBuffer
{
public:
	virtual void Bind() const override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual void UnBind() const override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	virtual void AssignTexture(ICachedTexture & texture, CachedTextureType type) override
	{
		throw std::logic_error("The method or operation is not implemented.");
	}
private:
};

CDirectXRenderer::CDirectXRenderer(ID3D11Device *dev, ID3D11DeviceContext *devcon)
	:m_dev(dev), m_devcon(devcon), m_defaultShaderManager(std::make_unique<CShaderManagerDirectX>(dev, devcon, nullptr)), m_textureManager(*this)
{
	m_defaultShaderManager->NewProgram();
}

void CDirectXRenderer::RenderArrays(RenderMode mode, std::vector<CVector2i> const& vertices, std::vector<CVector2f> const& texCoords)
{
	
}

void CDirectXRenderer::RenderArrays(RenderMode mode, std::vector<CVector2f> const& vertices, std::vector<CVector2f> const& texCoords)
{
	
}

void CDirectXRenderer::RenderArrays(RenderMode mode, std::vector<CVector3d> const& vertices, std::vector<CVector3d> const& normals, std::vector<CVector2d> const& texCoords)
{
	
}

void CDirectXRenderer::RenderArrays(RenderMode mode, std::vector<CVector3f> const& vertices, std::vector<CVector3f> const& normals, std::vector<CVector2f> const& texCoords)
{
	
}

void CDirectXRenderer::SetColor(const int * color)
{
	
}

void CDirectXRenderer::SetColor(const float * color)
{
	
}

void CDirectXRenderer::SetColor(const int r, const int g, const int b)
{
	
}

void CDirectXRenderer::SetColor(const float r, const float g, const float b)
{
	
}

void CDirectXRenderer::PushMatrix()
{
	
}

void CDirectXRenderer::PopMatrix()
{
	
}

void CDirectXRenderer::Translate(const int dx, const int dy, const int dz)
{
	
}

void CDirectXRenderer::Translate(const double dx, const double dy, const double dz)
{
	
}

void CDirectXRenderer::Translate(const float dx, const float dy, const float dz)
{
	
}

void CDirectXRenderer::Rotate(const double angle, const double x, const double y, const double z)
{
	
}

void CDirectXRenderer::Scale(const double scale)
{
	
}

void CDirectXRenderer::GetViewMatrix(float * matrix) const
{
	
}

void CDirectXRenderer::ResetViewMatrix()
{
	
}

void CDirectXRenderer::LookAt(CVector3d const& position, CVector3d const& direction, CVector3d const& up)
{
	
}

void CDirectXRenderer::SetTexture(std::string const& texture, const std::vector<sTeamColor> * teamcolor, int flags /*= 0*/)
{
	
}

void CDirectXRenderer::SetTexture(std::string const& texture, TextureSlot slot, int flags /*= 0*/)
{
	
}

void CDirectXRenderer::SetTexture(std::string const& texture, bool forceLoadNow /*= false*/, int flags /*= 0*/)
{
	
}

std::unique_ptr<ICachedTexture> CDirectXRenderer::RenderToTexture(std::function<void() > const& func, unsigned int width, unsigned int height)
{
	return std::make_unique<CDirectXCachedTexture>();
}

std::unique_ptr<ICachedTexture> CDirectXRenderer::CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type /*= CachedTextureType::RGBA*/)
{
	return std::make_unique<CDirectXCachedTexture>();
}

void CDirectXRenderer::SetMaterial(const float * ambient, const float * diffuse, const float * specular, const float shininess)
{
	
}

std::unique_ptr<IDrawingList> CDirectXRenderer::CreateDrawingList(std::function<void() > const& func)
{
	return std::make_unique<CDirectXDrawingList>();
}

std::unique_ptr<IVertexBuffer> CDirectXRenderer::CreateVertexBuffer(const float * vertex /*= nullptr*/, const float * normals /*= nullptr*/, const float * texcoords /*= nullptr*/)
{
	return std::make_unique<CDirectXVertexBuffer>();
}

std::unique_ptr<IShaderManager> CDirectXRenderer::CreateShaderManager() const
{
	return std::make_unique<CShaderManagerDirectX>(m_dev, m_devcon, m_defaultShaderManager.get());
}

void CDirectXRenderer::WindowCoordsToWorldVector(int x, int y, CVector3d & start, CVector3d & end) const
{
	
}

void CDirectXRenderer::WorldCoordsToWindowCoords(CVector3d const& worldCoords, int& x, int& y) const
{
	
}

std::unique_ptr<IFrameBuffer> CDirectXRenderer::CreateFramebuffer() const
{
	return std::make_unique<CDirectXFrameBuffer>();
}

void CDirectXRenderer::EnableLight(size_t index, bool enable)
{
	
}

void CDirectXRenderer::SetLightColor(size_t index, LightningType type, float * values)
{
	
}

void CDirectXRenderer::SetLightPosition(size_t index, float* pos)
{
	
}

float CDirectXRenderer::GetMaximumAnisotropyLevel() const
{
	return 1.0f;
}

void CDirectXRenderer::EnableVertexLightning(bool enable)
{
	
}

void CDirectXRenderer::GetProjectionMatrix(float * matrix) const
{
	
}

void CDirectXRenderer::EnableDepthTest(bool enable)
{
	
}

void CDirectXRenderer::EnableBlending(bool enable)
{
	
}

void CDirectXRenderer::SetUpViewport(CVector3d const& position, CVector3d const& target, unsigned int viewportWidth, unsigned int viewportHeight, double viewingAngle, double nearPane /*= 1.0*/, double farPane /*= 1000.0*/)
{
	
}

void CDirectXRenderer::RestoreViewport()
{
	
}

void CDirectXRenderer::EnablePolygonOffset(bool enable, float factor /*= 0.0f*/, float units /*= 0.0f*/)
{
	
}

void CDirectXRenderer::ClearBuffers(bool color /*= true*/, bool depth /*= true*/)
{
	ID3D11RenderTargetView * backbuffer;
	ID3D11DepthStencilView * depthBuffer;
	m_devcon->OMGetRenderTargets(1, &backbuffer, &depthBuffer);
	FLOAT backgroundColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	if(color) m_devcon->ClearRenderTargetView(backbuffer, backgroundColor);
	if (depth) m_devcon->ClearDepthStencilView(depthBuffer, 0, 0.0f, 0);
}

CTextureManager& CDirectXRenderer::GetTextureManager()
{
	return m_textureManager;
}

void CDirectXRenderer::ActivateTextureSlot(TextureSlot slot)
{
	
}

void CDirectXRenderer::UnbindTexture()
{
	
}

std::unique_ptr<ICachedTexture> CDirectXRenderer::CreateEmptyTexture()
{
	return std::make_unique<CDirectXCachedTexture>();
}

void CDirectXRenderer::SetTextureAnisotropy(float value /*= 1.0f*/)
{
	
}

void CDirectXRenderer::UploadTexture(unsigned char * data, unsigned int width, unsigned int height, unsigned short bpp, int flags, TextureMipMaps const& mipmaps /*= TextureMipMaps()*/)
{
	
}

void CDirectXRenderer::UploadCompressedTexture(unsigned char * data, unsigned int width, unsigned int height, size_t size, int flags, TextureMipMaps const& mipmaps /*= TextureMipMaps()*/)
{
	
}
