#include "OpenGLESRenderer.h"
#include "../LogWriter.h"
#include "../Utils.h"
#include "../view/IViewport.h"
#include "../view/TextureManager.h"
#include <GLES2/gl2ext.h> //GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, GL_COMPRESSED_RGB_S3TC_DXT1_EXT
#include <GLES3/gl3.h>

using namespace std;

namespace
{
class COpenGLESCachedTexture : public ICachedTexture
{
public:
	COpenGLESCachedTexture(GLenum type = GL_TEXTURE_2D);
	~COpenGLESCachedTexture();

	operator GLuint() const { return m_id; }
	GLenum GetType() const { return m_type; }

private:
	unsigned int m_id;
	GLenum m_type;
};

class COpenGLESVertexBuffer : public IVertexBuffer
{
public:
	COpenGLESVertexBuffer(COpenGLESRenderer& renderer, CShaderManagerOpenGLES& shaderMan, const float* vertex = nullptr, const float* normals = nullptr, const float* texcoords = nullptr, size_t size = 0, bool temp = true)
	{
		if (temp)
		{
			m_vertex = vertex;
			m_normals = normals;
			m_texCoords = texcoords;
			m_vertexCount = size;
		}
		else
		{
			glGenVertexArrays(1, &m_vao);
			renderer.BindVAO(m_vao, m_indexesBuffer);
			constexpr size_t vertexOffset = 0;
			const size_t normalOffset = vertexOffset + (vertex ? 3 : 0);
			const size_t texCoordOffset = normalOffset + (normals ? 3 : 0);
			const size_t stride = texCoordOffset + 2;
			vector<float> data(size * stride);
			for (size_t i = 0; i < size; ++i)
			{
				if (vertex)
					memcpy(data.data() + i * stride + vertexOffset, vertex + i * 3, sizeof(float) * 3);
				if (normals)
					memcpy(data.data() + i * stride + normalOffset, normals + i * 3, sizeof(float) * 3);
				if (texcoords)
					memcpy(data.data() + i * stride + texCoordOffset, texcoords + i * 2, sizeof(float) * 2);
			}
			m_buffer = shaderMan.CreateVertexAttribCache(data.size() * sizeof(float), data.data());
			shaderMan.SetInputAttributes(*m_buffer, 0, normalOffset * sizeof(float), texCoordOffset * sizeof(float), stride * sizeof(float));
		}
	}
	~COpenGLESVertexBuffer()
	{
		if (m_indexesBuffer)
			glDeleteBuffers(1, &m_indexesBuffer);
		if (m_vao)
			glDeleteVertexArrays(1, &m_vao);
	}
	void Bind(COpenGLESRenderer& renderer, CShaderManagerOpenGLES& shaderManager) const
	{
		renderer.BindVAO(m_vao, m_indexesBuffer);
		if (!m_vao)
		{
			shaderManager.SetInputAttributes(m_vertex, m_normals, m_texCoords, m_vertexCount, 3);
		}
	}
	void SetIndexBuffer(unsigned int* indexPtr, size_t indexesSize) override
	{
		glGenBuffers(1, &m_indexesBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexesBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexesSize * sizeof(unsigned), indexPtr, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

private:
	GLuint m_vao = 0;
	GLuint m_indexesBuffer = 0;
	std::unique_ptr<IVertexAttribCache> m_buffer;
	const float* m_vertex;
	const float* m_normals;
	const float* m_texCoords;
	size_t m_vertexCount;
};

class COpenGLESFrameBuffer : public IFrameBuffer
{
public:
	COpenGLESFrameBuffer()
	{
		glGenFramebuffers(1, &m_id);
		Bind();
	}

	~COpenGLESFrameBuffer()
	{
		UnBind();
		glDeleteBuffers(1, &m_id);
	}

	void Bind() const override
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_id);
	}

	void UnBind() const override
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void AssignTexture(ICachedTexture& texture, CachedTextureType type) override
	{
		static const std::map<CachedTextureType, GLenum> typeMap = {
			{ CachedTextureType::RGBA, GL_COLOR_ATTACHMENT0 },
			{ CachedTextureType::RENDER_TARGET, GL_COLOR_ATTACHMENT0 },
			{ CachedTextureType::ALPHA, GL_STENCIL_ATTACHMENT },
			{ CachedTextureType::DEPTH, GL_DEPTH_ATTACHMENT }
		};
		if (type == CachedTextureType::DEPTH)
		{
			GLenum buffers[] = { GL_NONE };
			glDrawBuffers(1, buffers);
			glReadBuffer(GL_NONE);
		}
		glFramebufferTexture2D(GL_FRAMEBUFFER, typeMap.at(type), GL_TEXTURE_2D, (COpenGLESCachedTexture&)texture, 0);
		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			throw std::runtime_error("Error creating framebuffer");
		}
	}

private:
	GLuint m_id;
};

class COpenGLESOcclusionQuery : public IOcclusionQuery
{
public:
	COpenGLESOcclusionQuery()
	{
	}
	~COpenGLESOcclusionQuery()
	{
		glDeleteQueries(1, &m_id);
	}
	void Query(std::function<void()> const& handler, bool renderToScreen) override
	{
		if (!m_id)
		{
			glGenQueries(1, &m_id);
		}
		if (!renderToScreen)
		{
			glDepthMask(GL_FALSE);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		}
		glBeginQuery(GL_ANY_SAMPLES_PASSED_CONSERVATIVE, m_id);
		handler();
		glEndQuery(GL_ANY_SAMPLES_PASSED_CONSERVATIVE);
		if (!renderToScreen)
		{
			glDepthMask(GL_TRUE);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		}
	}

	bool IsVisible() const override
	{
		GLuint result = 0;
		glGetQueryObjectuiv(m_id, GL_QUERY_RESULT_AVAILABLE, &result);
		if (result != 0)
		{
			glGetQueryObjectuiv(m_id, GL_QUERY_RESULT, &result);
			return result != 0;
		}
		return true;
	}

private:
	GLuint m_id = 0;
};

static const map<RenderMode, GLenum> renderModeMap = {
	{ RenderMode::TRIANGLES, GL_TRIANGLES },
	{ RenderMode::TRIANGLE_STRIP, GL_TRIANGLE_STRIP },
	{ RenderMode::LINES, GL_LINES },
	{ RenderMode::LINE_LOOP, GL_LINE_LOOP } //
};

#ifdef _WINDOWS
void APIENTRY ErrorCallback(GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, GLenum /*severity*/, GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
#else
void ErrorCallback(GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, GLenum /*severity*/, GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
#endif
{
	LogWriter::WriteLine(message);
}
}

COpenGLESCachedTexture::COpenGLESCachedTexture(GLenum type)
	: m_type(type)
{
	glGenTextures(1, &m_id);
}

COpenGLESCachedTexture::~COpenGLESCachedTexture()
{
	glDeleteTextures(1, &m_id);
}

COpenGLESRenderer::COpenGLESRenderer()
	: m_textureManager(nullptr)
	, m_version(3)
	, m_currentTextures(8)
{
}

COpenGLESRenderer::~COpenGLESRenderer()
{
	glDeleteVertexArrays(1, &m_vao);
}

void COpenGLESRenderer::RenderArrays(RenderMode mode, array_view<CVector3f> const& vertices, array_view<CVector3f> const& normals, array_view<CVector2f> const& texCoords)
{
	BindVAO(m_vao, 0);
	m_matrixManager.UpdateMatrices(m_shaderManager);
	m_shaderManager.SetInputAttributes(vertices.data(), normals.empty() ? nullptr : normals.data(), texCoords.empty() ? nullptr : (float*)texCoords.data(), vertices.size(), 3);
	glDrawArrays(renderModeMap.at(mode), 0, static_cast<GLsizei>(vertices.size()));
}

void COpenGLESRenderer::RenderArrays(RenderMode mode, array_view<CVector2i> const& vertices, array_view<CVector2f> const& texCoords)
{
	BindVAO(m_vao, 0);
	m_matrixManager.UpdateMatrices(m_shaderManager);
	vector<float> fvalues;
	fvalues.reserve(vertices.size() * 2);
	for (auto& v : vertices)
	{
		fvalues.push_back(static_cast<float>(v.x));
		fvalues.push_back(static_cast<float>(v.y));
	}
	m_shaderManager.SetInputAttributes(fvalues.data(), nullptr, texCoords.empty() ? nullptr : (float*)texCoords.data(), vertices.size(), 2);
	glDrawArrays(renderModeMap.at(mode), 0, static_cast<GLsizei>(vertices.size()));
}

void COpenGLESRenderer::DrawIndexes(IVertexBuffer& buffer, size_t begin, size_t count)
{
	m_matrixManager.UpdateMatrices(m_shaderManager);
	reinterpret_cast<COpenGLESVertexBuffer&>(buffer).Bind(*this, m_shaderManager);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(count), GL_UNSIGNED_INT, reinterpret_cast<void*>(begin * sizeof(unsigned int)));
}

void COpenGLESRenderer::DrawAll(IVertexBuffer& buffer, size_t count)
{
	m_matrixManager.UpdateMatrices(m_shaderManager);
	reinterpret_cast<COpenGLESVertexBuffer&>(buffer).Bind(*this, m_shaderManager);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(count));
}

void COpenGLESRenderer::DrawInstanced(IVertexBuffer& buffer, size_t size, size_t instanceCount)
{
	m_matrixManager.UpdateMatrices(m_shaderManager);
	reinterpret_cast<COpenGLESVertexBuffer&>(buffer).Bind(*this, m_shaderManager);
	glDrawArraysInstanced(GL_TRIANGLES, 0, static_cast<GLsizei>(size), static_cast<GLsizei>(instanceCount));
}

void COpenGLESRenderer::PushMatrix()
{
	m_matrixManager.PushMatrix();
}

void COpenGLESRenderer::PopMatrix()
{
	m_matrixManager.PopMatrix();
	m_matrixManager.UpdateMatrices(m_shaderManager);
}

void COpenGLESRenderer::Translate(const int dx, const int dy, const int dz)
{
	Translate(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz));
}

void COpenGLESRenderer::Translate(const double dx, const double dy, const double dz)
{
	Translate(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz));
}

void COpenGLESRenderer::Translate(const float dx, const float dy, const float dz)
{
	m_matrixManager.Translate(dx, dy, dz);
}

void COpenGLESRenderer::Scale(double scale)
{
	m_matrixManager.Scale(static_cast<float>(scale));
}

void COpenGLESRenderer::Rotate(double angle, double x, double y, double z)
{
	m_matrixManager.Rotate(angle, static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

void COpenGLESRenderer::GetViewMatrix(float* matrix) const
{
	m_matrixManager.GetModelViewMatrix(matrix);
}

void COpenGLESRenderer::LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up)
{
	m_matrixManager.LookAt(position, direction, up);
}

void COpenGLESRenderer::SetTexture(const Path& texture, bool forceLoadNow, int flags)
{
	if (forceLoadNow)
	{
		m_textureManager->LoadTextureNow(texture, nullptr, flags);
	}
	m_textureManager->SetTexture(texture, flags);
}

void COpenGLESRenderer::SetTexture(const Path& texture, TextureSlot slot, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, slot, nullptr, flags);
}

void COpenGLESRenderer::SetTexture(const Path& texture, const std::vector<sTeamColor>* teamcolor /*= nullptr*/, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, TextureSlot::eDiffuse, teamcolor, flags);
}

void COpenGLESRenderer::SetTexture(ICachedTexture const& texture, TextureSlot slot /*= TextureSlot::eDiffuse*/)
{
	auto& glTexture = reinterpret_cast<COpenGLESCachedTexture const&>(texture);
	unsigned slotIndex = static_cast<unsigned>(slot);
	if (m_currentTextures[slotIndex] == glTexture)
	{
		return;
	}
	m_currentTextures[slotIndex] = glTexture;
	if (slot != TextureSlot::eDiffuse)
		glActiveTexture(GL_TEXTURE0 + static_cast<int>(slot));
	glBindTexture(glTexture.GetType(), glTexture);
	if (slot != TextureSlot::eDiffuse)
		glActiveTexture(GL_TEXTURE0);
}

void COpenGLESRenderer::UnbindTexture(TextureSlot slot)
{
	unsigned slotIndex = static_cast<unsigned>(slot);
	if (m_currentTextures[slotIndex] == 0)
		return;
	if (slot != TextureSlot::eDiffuse)
		glActiveTexture(GL_TEXTURE0 + slotIndex);
	glBindTexture(GL_TEXTURE_2D, 0);
	if (slot != TextureSlot::eDiffuse)
		glActiveTexture(GL_TEXTURE0);
	m_currentTextures[slotIndex] = 0;
}

void COpenGLESRenderer::RenderToTexture(std::function<void()> const& func, ICachedTexture& tex, unsigned int width, unsigned int height)
{
	//set up buffer
	GLint prevBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBuffer);
	GLuint framebuffer = 0;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reinterpret_cast<COpenGLESCachedTexture&>(tex), 0);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		LogWriter::WriteLine("framebuffer error code=" + std::to_string(status));
	}
	int oldViewport[4];
	memcpy(oldViewport, m_viewport, sizeof(int) * 4);
	glViewport(0, 0, width, height);
	m_viewport[0] = 0;
	m_viewport[1] = 0;
	m_viewport[2] = width;
	m_viewport[3] = height;
	m_matrixManager.SaveMatrices();
	m_matrixManager.SetOrthographicProjection(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
	m_matrixManager.ResetModelView();

	glClear(GL_COLOR_BUFFER_BIT);
	func();

	m_matrixManager.RestoreMatrices();
	glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);

	glBindFramebuffer(GL_FRAMEBUFFER, prevBuffer);
	glDeleteFramebuffers(1, &framebuffer);
}

std::unique_ptr<ICachedTexture> COpenGLESRenderer::CreateTexture(const void* data, unsigned int width, unsigned int height, CachedTextureType type)
{
	//tuple<format, internalFormat, type>
	static const std::map<CachedTextureType, std::tuple<GLenum, GLenum, GLenum>> formatMap = {
		{ CachedTextureType::RGBA, std::tuple<GLenum, GLenum, GLenum>{ GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE } },
		{ CachedTextureType::RENDER_TARGET, std::tuple<GLenum, GLenum, GLenum>{ GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE } },
		{ CachedTextureType::ALPHA, std::tuple<GLenum, GLenum, GLenum>{ GL_ALPHA, GL_ALPHA, GL_UNSIGNED_BYTE } },
		{ CachedTextureType::DEPTH, std::tuple<GLenum, GLenum, GLenum>{ GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT } }
	};
	auto texture = std::make_unique<COpenGLESCachedTexture>();
	SetTexture(*texture);
	glTexImage2D(GL_TEXTURE_2D, 0, std::get<1>(formatMap.at(type)), width, height, 0, std::get<0>(formatMap.at(type)), std::get<2>(formatMap.at(type)), data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (type == CachedTextureType::DEPTH && m_version >= 3)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	}
	return move(texture);
}

ICachedTexture* COpenGLESRenderer::GetTexturePtr(const Path& texture) const
{
	return m_textureManager->GetTexturePtr(texture);
}

void COpenGLESRenderer::SetColor(const float r, const float g, const float b, const float a)
{
	float color[] = { r, g, b, a };
	SetColor(color);
}

void COpenGLESRenderer::SetColor(const int r, const int g, const int b, const int a)
{
	int color[] = { r, g, b, a };
	SetColor(color);
}

void COpenGLESRenderer::SetColor(const float* color)
{
	memcpy(m_color, color, sizeof(float) * 4);
	static const std::string colorKey = "color";
	m_shaderManager.SetUniformValue(colorKey, 4, 1, m_color);
}

void COpenGLESRenderer::SetColor(const int* color)
{
	auto charToFloat = [](const int value) { return static_cast<float>(value) / UCHAR_MAX; };
	float fcolor[] = { charToFloat(color[0]), charToFloat(color[1]), charToFloat(color[2]), 1.0f };
	SetColor(fcolor);
}

void COpenGLESRenderer::SetMaterial(const float* ambient, const float* diffuse, const float* specular, const float shininess)
{
	m_shaderManager.SetMaterial(ambient, diffuse, specular, shininess);
}

std::unique_ptr<IVertexBuffer> COpenGLESRenderer::CreateVertexBuffer(const float* vertex, const float* normals, const float* texcoords, size_t size, bool temp)
{
	return std::make_unique<COpenGLESVertexBuffer>(*this, m_shaderManager, vertex, normals, texcoords, size, temp);
}

std::unique_ptr<IOcclusionQuery> COpenGLESRenderer::CreateOcclusionQuery()
{
	return std::make_unique<COpenGLESOcclusionQuery>();
}

std::string COpenGLESRenderer::GetName() const
{
	return "OpenGLES";
}

bool COpenGLESRenderer::SupportsFeature(Feature /*feature*/) const
{
	return true;
}

IShaderManager& COpenGLESRenderer::GetShaderManager()
{
	return m_shaderManager;
}

std::unique_ptr<ICachedTexture> COpenGLESRenderer::CreateEmptyTexture(bool cubemap)
{
	return std::make_unique<COpenGLESCachedTexture>(cubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
}

void COpenGLESRenderer::SetTextureAnisotropy(float value)
{
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);
}

void COpenGLESRenderer::UploadTexture(ICachedTexture& texture, unsigned char* data, size_t width, size_t height, unsigned short /*bpp*/, int flags, TextureMipMaps const& mipmaps)
{
	SetTexture(texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_BUILD_MIPMAPS || !mipmaps.empty()) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	GLenum format = (flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, (flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	if (flags & TEXTURE_BUILD_MIPMAPS)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	for (size_t i = 0; i < mipmaps.size(); i++)
	{
		auto& mipmap = mipmaps[i];
		glTexImage2D(GL_TEXTURE_2D, i + 1, (flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB, mipmap.width, mipmap.height, 0, format, GL_UNSIGNED_BYTE, mipmap.data);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmaps.size());
}

void COpenGLESRenderer::UploadCompressedTexture(ICachedTexture& texture, unsigned char* data, size_t width, size_t height, size_t size, int flags, TextureMipMaps const& mipmaps)
{
	SetTexture(texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_BUILD_MIPMAPS || !mipmaps.empty()) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

	static const std::map<int, int> compressionMap = {
		{ TEXTURE_COMPRESSION_DXT1_NO_ALPHA, GL_COMPRESSED_RGB_S3TC_DXT1_EXT },
		{ TEXTURE_COMPRESSION_DXT1, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT },
		{ TEXTURE_COMPRESSION_DXT3, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT },
		{ TEXTURE_COMPRESSION_DXT5, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT }
	};
	GLenum format = compressionMap.at(flags & TEXTURE_COMPRESSION_MASK);

	glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, size, data);

	for (size_t i = 0; i < mipmaps.size(); i++)
	{
		auto& mipmap = mipmaps[i];
		glCompressedTexImage2D(GL_TEXTURE_2D, i + 1, format, mipmap.width, mipmap.height, 0, mipmap.size, mipmap.data);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmaps.size());
}

void COpenGLESRenderer::UploadCubemap(ICachedTexture& texture, TextureMipMaps const& sides, unsigned short, int flags)
{
	SetTexture(texture);
	GLenum format = (flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB;
	for (size_t i = 0; i < sides.size(); ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, (flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB, static_cast<GLsizei>(sides[i].width), static_cast<GLsizei>(sides[i].height), 0, format, GL_UNSIGNED_BYTE, sides[i].data);
	}
	flags &= ~TEXTURE_BUILD_MIPMAPS;
	if (flags & TEXTURE_BUILD_MIPMAPS)
	{
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_BUILD_MIPMAPS) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	UnbindTexture();
}

bool COpenGLESRenderer::Force32Bits() const
{
	return false;
}

bool COpenGLESRenderer::ForceFlipBMP() const
{
	return false;
}

bool COpenGLESRenderer::ConvertBgra() const
{
	return true;
}

std::unique_ptr<IFrameBuffer> COpenGLESRenderer::CreateFramebuffer() const
{
	return std::make_unique<COpenGLESFrameBuffer>();
}

void COpenGLESRenderer::SetTextureManager(CTextureManager& textureManager)
{
	m_textureManager = &textureManager;
}

void COpenGLESRenderer::WindowCoordsToWorldVector(IViewport& viewport, int x, int y, CVector3f& start, CVector3f& end) const
{
	m_matrixManager.WindowCoordsToWorldVector(x, y, (float)viewport.GetX(), (float)viewport.GetY(), (float)viewport.GetWidth(), (float)viewport.GetHeight(), viewport.GetViewMatrix(), viewport.GetProjectionMatrix(), start, end);
}

void COpenGLESRenderer::WorldCoordsToWindowCoords(IViewport& viewport, CVector3f const& worldCoords, int& x, int& y) const
{
	m_matrixManager.WorldCoordsToWindowCoords(worldCoords, (float)viewport.GetX(), (float)viewport.GetY(), (float)viewport.GetWidth(), (float)viewport.GetHeight(), viewport.GetViewMatrix(), viewport.GetProjectionMatrix(), x, y);
}

void COpenGLESRenderer::SetNumberOfLights(size_t count)
{
	static const std::string numberOfLightsKey = "lightsCount";
	int number = static_cast<int>(count);
	m_shaderManager.SetUniformValue(numberOfLightsKey, 1, 1, &number);
}

void COpenGLESRenderer::SetUpLight(size_t index, CVector3f const& position, const float* ambient, const float* diffuse, const float* specular)
{
	const std::string key = "lights[" + std::to_string(index) + "].";
	m_shaderManager.SetUniformValue(key + "pos", 3, 1, position.ptr());
	m_shaderManager.SetUniformValue(key + "ambient", 4, 1, ambient);
	m_shaderManager.SetUniformValue(key + "diffuse", 4, 1, diffuse);
	m_shaderManager.SetUniformValue(key + "specular", 4, 1, specular);
}

float COpenGLESRenderer::GetMaximumAnisotropyLevel() const
{
	float aniso = 16.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	return aniso;
}

void COpenGLESRenderer::GetProjectionMatrix(float* matrix) const
{
	m_matrixManager.GetProjectionMatrix(matrix);
}

void COpenGLESRenderer::EnableDepthTest(bool enable)
{
	if (enable)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void COpenGLESRenderer::EnableBlending(bool enable)
{
	if (enable)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
}

void COpenGLESRenderer::SetUpViewport(unsigned int viewportX, unsigned int viewportY, unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane, float farPane)
{
	m_matrixManager.SetUpViewport(viewportWidth * m_vrViewport[2], viewportHeight * m_vrViewport[3], m_vrFovOverride > 0.00001f ? m_vrFovOverride : viewingAngle, nearPane, farPane);
	glViewport(viewportX + m_vrViewport[0] * viewportWidth, viewportY + m_vrViewport[1] * viewportHeight, viewportWidth * m_vrViewport[2], viewportHeight * m_vrViewport[3]);
	m_viewport[0] = viewportX;
	m_viewport[1] = viewportY;
	m_viewport[2] = viewportWidth;
	m_viewport[3] = viewportHeight;
}

void COpenGLESRenderer::EnablePolygonOffset(bool enable, float factor /*= 0.0f*/, float units /*= 0.0f*/)
{
	if (enable)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(factor, units);
	}
	else
	{
		glPolygonOffset(0.0f, 0.0f);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}

void COpenGLESRenderer::ClearBuffers(bool color, bool depth)
{
	if (!m_disableClear)
	{
		GLbitfield mask = 0;
		if (color)
			mask |= GL_COLOR_BUFFER_BIT;
		if (depth)
			mask |= GL_DEPTH_BUFFER_BIT;
		glClear(mask);
	}
}

void COpenGLESRenderer::DrawIn2D(std::function<void()> const& drawHandler)
{
	m_matrixManager.SaveMatrices();
	m_matrixManager.SetOrthographicProjection(static_cast<float>(m_viewport[0]), static_cast<float>(m_viewport[2]), static_cast<float>(m_viewport[3]), static_cast<float>(m_viewport[1]));
	m_matrixManager.ResetModelView();

	drawHandler();

	m_matrixManager.RestoreMatrices();
}

void COpenGLESRenderer::SetVersion(int version)
{
	m_version = version;
}

void COpenGLESRenderer::Init(int width, int height)
{
#if not defined(NDEBUG) and defined(GL_ES_VERSION_3_2)
	glDebugMessageCallback(ErrorCallback, nullptr);
#endif
	glDepthFunc(GL_LESS);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, width, height);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	m_color[3] = 1.0f;
	m_shaderManager.DoOnProgramChange([this]() {
		m_matrixManager.UpdateMatrices(m_shaderManager);
	});
	m_defaultProgram = m_shaderManager.NewProgram();
	m_shaderManager.PushProgram(*m_defaultProgram);
}

void COpenGLESRenderer::BindVAO(unsigned vao, unsigned indexBuffer)
{
	if (vao == 0)
		vao = m_vao;
	if (m_activeVao != vao)
	{
		glBindVertexArray(vao);
		m_activeVao = vao;
	}
	if (m_indexBuffer != indexBuffer)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		m_indexBuffer = indexBuffer;
	}
}

void COpenGLESRenderer::SetVrViewport(float x, float y, float width, float height, float fov)
{
	m_vrViewport[0] = x;
	m_vrViewport[1] = y;
	m_vrViewport[2] = width;
	m_vrViewport[3] = height;
	m_vrFovOverride = fov;
}

void COpenGLESRenderer::SetVrViewMatrices(std::vector<const float*> const& matrices)
{
	m_matrixManager.SetVrViewMatrices(matrices);
}

void COpenGLESRenderer::DisableClear(bool disable)
{
	m_disableClear = disable;
}