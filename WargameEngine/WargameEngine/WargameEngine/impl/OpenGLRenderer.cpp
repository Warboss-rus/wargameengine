#include "OpenGLRenderer.h"
#include "../LogWriter.h"
#include "../view/IViewport.h"
#include "../view/TextureManager.h"
#include <GL/glew.h>
#include "gl.h"

using namespace std;

namespace
{
class COpenGLVertexBuffer : public IVertexBuffer
{
public:
	COpenGLVertexBuffer(COpenGLRenderer& renderer, CShaderManagerOpenGL& shaderMan, const float* vertex = nullptr, const float* normals = nullptr, const float* texcoords = nullptr, size_t size = 0, bool temp = true)
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
			m_cache = shaderMan.CreateVertexAttribCache(data.size() * sizeof(float), data.data());
			shaderMan.SetInputAttributes(*m_cache, 0, normalOffset * sizeof(float), texCoordOffset * sizeof(float), stride * sizeof(float));
		}
	}

	~COpenGLVertexBuffer()
	{
		if (m_indexesBuffer)
			glDeleteBuffers(1, &m_indexesBuffer);
		if (m_vao)
			glDeleteVertexArrays(1, &m_vao);
	}

	void SetIndexBuffer(GLuint indexBuffer)
	{
		m_indexesBuffer = indexBuffer;
	}

	void Bind(COpenGLRenderer& renderer, CShaderManagerOpenGL& shaderManager) const
	{
		renderer.BindVAO(m_vao, m_indexesBuffer);
		if (!m_vao)
		{
			shaderManager.SetInputAttributes(m_vertex, m_normals, m_texCoords, m_vertexCount, 3);
		}
	}

private:
	GLuint m_vao = 0;
	GLuint m_indexesBuffer = 0;
	unique_ptr<IVertexAttribCache> m_cache;
	const float* m_vertex;
	const float* m_normals;
	const float* m_texCoords;
	size_t m_vertexCount;
};

class COpenGLFrameBuffer : public IFrameBuffer
{
public:
	COpenGLFrameBuffer()
	{
		glGenFramebuffers(1, &m_id);
		Bind();
	}

	~COpenGLFrameBuffer()
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
		static const map<CachedTextureType, GLenum> typeMap = {
			{ CachedTextureType::RGBA, GL_COLOR_ATTACHMENT0 },
			{ CachedTextureType::ALPHA, GL_STENCIL_ATTACHMENT },
			{ CachedTextureType::DEPTH, GL_DEPTH_ATTACHMENT }
		};
		const map<CachedTextureType, pair<GLboolean, string>> extensionMap = {
			{ CachedTextureType::RGBA, { GLEW_ARB_color_buffer_float, "GL_ARB_color_buffer_float" } },
			{ CachedTextureType::ALPHA, { GLEW_ARB_stencil_texturing, "GL_ARB_stencil_texturing" } },
			{ CachedTextureType::DEPTH, { GLEW_ARB_depth_buffer_float, "GL_ARB_depth_buffer_float" } }
		};
		if (!extensionMap.at(type).first)
		{
			throw runtime_error(extensionMap.at(type).second + " is not supported");
		}
		if (type == CachedTextureType::DEPTH)
		{
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
		glFramebufferTexture2D(GL_FRAMEBUFFER, typeMap.at(type), GL_TEXTURE_2D, (COpenGlCachedTexture&)texture, 0);
		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			throw runtime_error("Error creating framebuffer");
		}
	}

private:
	GLuint m_id;
};

class COpenGLOcclusionQuery : public IOcclusionQuery
{
public:
	COpenGLOcclusionQuery()
	{
	}
	~COpenGLOcclusionQuery()
	{
		glDeleteQueries(1, &m_id);
	}
	void Query(function<void()> const& handler, bool renderToScreen) override
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
		if (!m_id)
			return true;
		if (GLEW_ARB_query_buffer_object)
		{
			int result = 1; //true by default
			glGetQueryObjectiv(m_id, GL_QUERY_RESULT_NO_WAIT, &result);
			return result != 0;
		}
		else
		{
			GLint result = 0;
			glGetQueryObjectiv(m_id, GL_QUERY_RESULT_AVAILABLE, &result);
			if (result != 0)
			{
				glGetQueryObjectiv(m_id, GL_QUERY_RESULT, &result);
				return result != 0;
			}
			return true;
		}
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

COpenGlCachedTexture::COpenGlCachedTexture(unsigned int type, bool dsa)
	: m_type(type)
{
	if (dsa)
	{
		glCreateTextures(type, 1, &m_id);
	}
	else
	{
		glGenTextures(1, &m_id);
	}
}

COpenGlCachedTexture::~COpenGlCachedTexture()
{
	glDeleteTextures(1, &m_id);
}

COpenGLRenderer::COpenGLRenderer()
	: m_textureManager(nullptr)
	, m_currentTextures(8)
{
	if (glewInit() != GLEW_OK || !GLEW_VERSION_3_0)
	{
		throw runtime_error("failed to initialize GLEW");
	}
	glDepthFunc(GL_LESS);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifdef _DEBUG
	if (GLEW_KHR_debug)
	{
		glDebugMessageCallback(ErrorCallback, nullptr);
	}
#endif

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_color[3] = 1.0f;
	m_shaderManager.DoOnProgramChange([this]() {
		m_matrixManager.InvalidateMatrices();
	});

	m_defaultProgram = m_shaderManager.NewProgram();
	m_shaderManager.PushProgram(*m_defaultProgram);
	if (GLEW_ARB_seamless_cube_map)
	{
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}
	m_supportsMultibind = GLEW_ARB_multi_bind;
	m_supportsDSA = GLEW_ARB_direct_state_access;
}

COpenGLRenderer::~COpenGLRenderer()
{
	glDeleteVertexArrays(1, &m_vao);
}

void COpenGLRenderer::RenderArrays(RenderMode mode, array_view<CVector3f> const& vertices, array_view<CVector3f> const& normals, array_view<CVector2f> const& texCoords)
{
	BindVAO(m_vao, NULL);
	m_matrixManager.UpdateMatrices(m_shaderManager);
	m_shaderManager.SetInputAttributes(vertices.data(), normals.empty() ? nullptr : normals.data(), texCoords.empty() ? nullptr : (float*)texCoords.data(), vertices.size(), 3);
	glDrawArrays(renderModeMap.at(mode), 0, static_cast<GLsizei>(vertices.size()));
}

void COpenGLRenderer::RenderArrays(RenderMode mode, array_view<CVector2i> const& vertices, array_view<CVector2f> const& texCoords)
{
	BindVAO(m_vao, NULL);
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

void COpenGLRenderer::DrawIndexes(IVertexBuffer& vertexBuffer, size_t begin, size_t count)
{
	m_matrixManager.UpdateMatrices(m_shaderManager);
	reinterpret_cast<COpenGLVertexBuffer&>(vertexBuffer).Bind(*this, m_shaderManager);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(count), GL_UNSIGNED_INT, reinterpret_cast<void*>(begin * sizeof(unsigned int)));
}

void COpenGLRenderer::DrawAll(IVertexBuffer& vertexBuffer, size_t count)
{
	m_matrixManager.UpdateMatrices(m_shaderManager);
	reinterpret_cast<COpenGLVertexBuffer&>(vertexBuffer).Bind(*this, m_shaderManager);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(count));
}

void COpenGLRenderer::DrawInstanced(IVertexBuffer& vertexBuffer, size_t size, size_t instanceCount)
{
	m_matrixManager.UpdateMatrices(m_shaderManager);
	reinterpret_cast<COpenGLVertexBuffer&>(vertexBuffer).Bind(*this, m_shaderManager);
	glDrawArraysInstanced(GL_TRIANGLES, 0, static_cast<GLsizei>(size), static_cast<GLsizei>(instanceCount));
}

void COpenGLRenderer::SetIndexBuffer(IVertexBuffer& buffer, const unsigned int* indexPtr, size_t indexesSize)
{
	GLuint indexBuffer;
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexesSize * sizeof(unsigned), indexPtr, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	reinterpret_cast<COpenGLVertexBuffer&>(buffer).SetIndexBuffer(indexBuffer);
}

void COpenGLRenderer::PushMatrix()
{
	m_matrixManager.PushMatrix();
}

void COpenGLRenderer::PopMatrix()
{
	m_matrixManager.PopMatrix();
	m_matrixManager.UpdateMatrices(m_shaderManager);
}

void COpenGLRenderer::Translate(int dx, int dy, int dz)
{
	m_matrixManager.Translate(static_cast<float>(dx), static_cast<float>(dy), static_cast<float>(dz));
}

void COpenGLRenderer::Translate(const CVector3f& delta)
{
	m_matrixManager.Translate(delta.x, delta.y, delta.z);
}

void COpenGLRenderer::Scale(float scale)
{
	m_matrixManager.Scale(scale);
}

void COpenGLRenderer::Rotate(float angle, const CVector3f& axis)
{
	m_matrixManager.Rotate(angle, axis);
}

void COpenGLRenderer::Rotate(const CVector3f& rotations)
{
	m_matrixManager.Rotate(rotations);
}

const float* COpenGLRenderer::GetViewMatrix() const
{
	return m_matrixManager.GetModelViewMatrix();
}

void COpenGLRenderer::LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up)
{
	m_matrixManager.LookAt(position, direction, up);
}

void COpenGLRenderer::SetTexture(const Path& texture, bool forceLoadNow, int flags)
{
	if (forceLoadNow)
	{
		m_textureManager->LoadTextureNow(texture, nullptr, flags);
	}
	m_textureManager->SetTexture(texture, flags);
}

void COpenGLRenderer::SetTexture(const Path& texture, TextureSlot slot, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, slot, nullptr, flags);
}

void COpenGLRenderer::SetTexture(const Path& texture, const vector<sTeamColor>* teamcolor /*= nullptr*/, int flags /*= 0*/)
{
	m_textureManager->SetTexture(texture, TextureSlot::eDiffuse, teamcolor, flags);
}

void COpenGLRenderer::SetTexture(ICachedTexture const& texture, TextureSlot slot)
{
	auto& glTexture = reinterpret_cast<COpenGlCachedTexture const&>(texture);
	unsigned slotIndex = static_cast<unsigned>(slot);
	if (m_currentTextures[slotIndex] == glTexture)
	{
		return;
	}
	m_currentTextures[slotIndex] = glTexture;
	if (m_supportsDSA)
	{
		glBindTextureUnit(slotIndex, glTexture);
	}
	else if (m_supportsMultibind)
	{
		GLuint tex = glTexture;
		glBindTextures(GL_TEXTURE0 + slotIndex, 1, &tex);
	}
	else
	{
		if (slot != TextureSlot::eDiffuse)
			glActiveTexture(GL_TEXTURE0 + static_cast<int>(slot));
		glBindTexture(glTexture.GetType(), glTexture);
		if (slot != TextureSlot::eDiffuse)
			glActiveTexture(GL_TEXTURE0);
	}
}

void COpenGLRenderer::UnbindTexture(TextureSlot slot)
{
	unsigned slotIndex = static_cast<unsigned>(slot);
	if (m_currentTextures[slotIndex] == 0)
		return;
	if (m_supportsDSA)
	{
		glBindTextureUnit(slotIndex, 0);
	}
	else if (m_supportsMultibind)
	{
		GLuint tex = 0;
		glBindTextures(GL_TEXTURE0 + slotIndex, 1, &tex);
	}
	else
	{
		if (slot != TextureSlot::eDiffuse)
			glActiveTexture(GL_TEXTURE0 + slotIndex);
		glBindTexture(GL_TEXTURE_2D, 0);
		if (slot != TextureSlot::eDiffuse)
			glActiveTexture(GL_TEXTURE0);
	}
	m_currentTextures[slotIndex] = 0;
}

void COpenGLRenderer::RenderToTexture(function<void()> const& func, ICachedTexture& tex, unsigned int width, unsigned int height)
{
	//set up buffer
	GLint prevBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBuffer);
	GLuint framebuffer = 0;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reinterpret_cast<COpenGlCachedTexture&>(tex), 0);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		LogWriter::WriteLine("framebuffer error code=" + to_string(status));
	}
	int oldViewport[4];
	memcpy(oldViewport, m_viewport, sizeof(int) * 4);
	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	m_viewport[0] = 0;
	m_viewport[1] = 0;
	m_viewport[2] = static_cast<int>(width);
	m_viewport[3] = static_cast<int>(height);
	m_matrixManager.SaveMatrices();
	m_matrixManager.SetOrthographicProjection(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
	m_matrixManager.ResetModelView();

	glClear(GL_COLOR_BUFFER_BIT);
	func();

	m_matrixManager.RestoreMatrices();
	glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]);
	memcpy(m_viewport, oldViewport, sizeof(int) * 4);

	glBindFramebuffer(GL_FRAMEBUFFER, prevBuffer);
	glDeleteFramebuffers(1, &framebuffer);
}

unique_ptr<ICachedTexture> COpenGLRenderer::CreateTexture(const void* data, unsigned int width, unsigned int height, CachedTextureType type)
{
	//tuple<format, internalFormat, type>
	static const map<CachedTextureType, tuple<GLenum, GLenum, GLenum>> formatMap = {
		{ CachedTextureType::RGBA, { GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE } },
		{ CachedTextureType::RENDER_TARGET, { GL_RGBA, GL_RGBA8, GL_UNSIGNED_BYTE } },
		{ CachedTextureType::ALPHA, { GL_RED, GL_R8, GL_UNSIGNED_BYTE } },
		{ CachedTextureType::DEPTH, { GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32, GL_UNSIGNED_INT } }
	};

	auto texture = make_unique<COpenGlCachedTexture>(GL_TEXTURE_2D, m_supportsDSA);
	if (width == 0 || height == 0)
	{
		return move(texture);
	}
	if (m_supportsDSA)
	{
		unsigned glTexture = *texture;
		glTextureStorage2D(glTexture, 1, get<1>(formatMap.at(type)), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
		if (data)
		{
			glTextureSubImage2D(glTexture, 0, 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), get<0>(formatMap.at(type)), get<2>(formatMap.at(type)), data);
		}

		if (type == CachedTextureType::ALPHA)
		{
			GLint swizzleMask[] = { GL_ZERO, GL_ZERO, GL_ZERO, GL_RED };
			glTextureParameteriv(glTexture, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
		}
		glTextureParameteri(glTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(glTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(glTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(glTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if (type == CachedTextureType::DEPTH)
		{
			glTextureParameteri(*texture, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTextureParameteri(*texture, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		}
	}
	else
	{
		SetTexture(*texture);
		glTexImage2D(GL_TEXTURE_2D, 0, get<1>(formatMap.at(type)), static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, get<0>(formatMap.at(type)), get<2>(formatMap.at(type)), data);
		if (type == CachedTextureType::ALPHA)
		{
			GLint swizzleMask[] = { GL_ZERO, GL_ZERO, GL_ZERO, GL_RED };
			glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if (type == CachedTextureType::DEPTH)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		}
	}
	return move(texture);
}

ICachedTexture* COpenGLRenderer::GetTexturePtr(wstring const& texture) const
{
	return m_textureManager->GetTexturePtr(texture);
}

void COpenGLRenderer::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	auto charToFloat = [](const int value) { return static_cast<float>(value) / 0xff; };
	const float color[] = { charToFloat(r), charToFloat(g), charToFloat(b), charToFloat(a) };
	SetColor(color);
}

void COpenGLRenderer::SetColor(const float* color)
{
	memcpy(m_color, color, sizeof(float) * 4);
	m_shaderManager.SetUniformValue("color", 4, 1, m_color);
}

void COpenGLRenderer::SetMaterial(const float* ambient, const float* diffuse, const float* specular, const float shininess)
{
	m_shaderManager.SetMaterial(ambient, diffuse, specular, shininess);
}

unique_ptr<IVertexBuffer> COpenGLRenderer::CreateVertexBuffer(const float* vertex, const float* normals, const float* texcoords, size_t size, bool temp)
{
	return make_unique<COpenGLVertexBuffer>(*this, m_shaderManager, vertex, normals, texcoords, size, temp);
}

unique_ptr<IOcclusionQuery> COpenGLRenderer::CreateOcclusionQuery()
{
	return make_unique<COpenGLOcclusionQuery>();
}

string COpenGLRenderer::GetName() const
{
	return "OpenGL";
}

bool COpenGLRenderer::SupportsFeature(Feature feature) const
{
	if (feature == Feature::INSTANSING)
	{
		return GLEW_VERSION_3_1 != GL_FALSE;
	}
	return true;
}

IShaderManager& COpenGLRenderer::GetShaderManager()
{
	return m_shaderManager;
}

unique_ptr<ICachedTexture> COpenGLRenderer::CreateEmptyTexture(bool cubemap)
{
	return make_unique<COpenGlCachedTexture>(cubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, m_supportsDSA);
}

void COpenGLRenderer::SetTextureAnisotropy(float value)
{
	if (GLEW_EXT_texture_filter_anisotropic)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);
	}
}

void COpenGLRenderer::UploadTexture(ICachedTexture& texture, unsigned char* data, size_t width, size_t height, unsigned short, int flags, TextureMipMaps const& mipmaps)
{
	GLenum format = (flags & TEXTURE_BGRA) ? ((flags & TEXTURE_HAS_ALPHA) ? GL_BGRA : GL_BGR_EXT) : ((flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB);
	if (m_supportsDSA)
	{
		unsigned glTexture = reinterpret_cast<COpenGlCachedTexture&>(texture);
		glTextureParameteri(glTexture, GL_TEXTURE_WRAP_S, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTextureParameteri(glTexture, GL_TEXTURE_WRAP_T, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTextureParameteri(glTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(glTexture, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_BUILD_MIPMAPS || !mipmaps.empty()) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTextureParameteri(glTexture, GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(glTexture, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(mipmaps.size()));
		glTextureStorage2D(glTexture, 1, (flags & TEXTURE_HAS_ALPHA) ? GL_RGBA8 : GL_RGB8, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
		glTextureSubImage2D(glTexture, 0, 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), format, GL_UNSIGNED_BYTE, data);
		if (flags & TEXTURE_BUILD_MIPMAPS)
		{
			glGenerateTextureMipmap(glTexture);
		}
		for (size_t i = 0; i < mipmaps.size(); i++)
		{
			auto& mipmap = mipmaps[i];
			glTextureSubImage2D(glTexture, i + 1, (flags & TEXTURE_HAS_ALPHA) ? GL_RGBA8 : GL_RGB8, static_cast<GLsizei>(mipmap.width), static_cast<GLsizei>(mipmap.height), 0, format, GL_UNSIGNED_BYTE, mipmap.data);
		}
	}
	else
	{
		SetTexture(texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_BUILD_MIPMAPS || !mipmaps.empty()) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, (flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, format, GL_UNSIGNED_BYTE, data);
		if (flags & TEXTURE_BUILD_MIPMAPS)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		for (size_t i = 0; i < mipmaps.size(); i++)
		{
			auto& mipmap = mipmaps[i];
			glTexImage2D(GL_TEXTURE_2D, i + 1, (flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB, static_cast<GLsizei>(mipmap.width), static_cast<GLsizei>(mipmap.height), 0, format, GL_UNSIGNED_BYTE, mipmap.data);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(mipmaps.size()));
	}
}

void COpenGLRenderer::UploadCompressedTexture(ICachedTexture& texture, unsigned char* data, size_t width, size_t height, size_t size, int flags, TextureMipMaps const& mipmaps)
{
	if (!GLEW_EXT_texture_compression_s3tc)
	{
		LogWriter::WriteLine("Compressed textures are not supported");
		return;
	}
	static const map<int, int> compressionMap = {
		{ TEXTURE_COMPRESSION_DXT1_NO_ALPHA, GL_COMPRESSED_RGB_S3TC_DXT1_EXT },
		{ TEXTURE_COMPRESSION_DXT1, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT },
		{ TEXTURE_COMPRESSION_DXT3, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT },
		{ TEXTURE_COMPRESSION_DXT5, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT }
	};
	GLenum format = compressionMap.at(flags & TEXTURE_COMPRESSION_MASK);
	if (m_supportsDSA)
	{
		GLuint glTexture = reinterpret_cast<COpenGlCachedTexture&>(texture);
		glTextureParameteri(glTexture, GL_TEXTURE_WRAP_S, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTextureParameteri(glTexture, GL_TEXTURE_WRAP_T, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTextureParameteri(glTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(glTexture, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_BUILD_MIPMAPS || !mipmaps.empty()) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glTextureParameteri(glTexture, GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(glTexture, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(mipmaps.size()));

		glTextureStorage2D(glTexture, mipmaps.size() + 1, format, width, height);
		glCompressedTextureSubImage2D(glTexture, 0, 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height), format, static_cast<GLsizei>(size), data);

		for (size_t i = 0; i < mipmaps.size(); i++)
		{
			auto& mipmap = mipmaps[i];
			glCompressedTextureSubImage2D(glTexture, i + 1, 0, 0, static_cast<GLsizei>(mipmap.width), static_cast<GLsizei>(mipmap.height), format, static_cast<GLsizei>(mipmap.size), mipmap.data);
		}
	}
	else
	{
		SetTexture(texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_BUILD_MIPMAPS || !mipmaps.empty()) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

		glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, static_cast<GLsizei>(size), data);

		for (size_t i = 0; i < mipmaps.size(); i++)
		{
			auto& mipmap = mipmaps[i];
			glCompressedTexImage2D(GL_TEXTURE_2D, i + 1, format, static_cast<GLsizei>(mipmap.width), static_cast<GLsizei>(mipmap.height), 0, static_cast<GLsizei>(mipmap.size), mipmap.data);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(mipmaps.size()));
	}
}

void COpenGLRenderer::UploadCubemap(ICachedTexture& texture, TextureMipMaps const& sides, unsigned short, int flags)
{
	SetTexture(texture);
	GLenum format = (flags & TEXTURE_BGRA) ? ((flags & TEXTURE_HAS_ALPHA) ? GL_BGRA : GL_BGR_EXT) : ((flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB);
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

bool COpenGLRenderer::Force32Bits() const
{
	return false;
}

bool COpenGLRenderer::ForceFlipBMP() const
{
	return false;
}

bool COpenGLRenderer::ConvertBgra() const
{
	return false;
}

unique_ptr<IFrameBuffer> COpenGLRenderer::CreateFramebuffer() const
{
	return make_unique<COpenGLFrameBuffer>();
}

void COpenGLRenderer::SetTextureManager(CTextureManager& textureManager)
{
	m_textureManager = &textureManager;
}

void COpenGLRenderer::WindowCoordsToWorldVector(IViewport& viewport, int x, int y, CVector3f& start, CVector3f& end) const
{
	m_matrixManager.WindowCoordsToWorldVector(x, y, (float)viewport.GetX(), (float)viewport.GetY(), (float)viewport.GetWidth(), (float)viewport.GetHeight(), viewport.GetViewMatrix(), viewport.GetProjectionMatrix(), start, end);
}

void COpenGLRenderer::WorldCoordsToWindowCoords(IViewport& viewport, CVector3f const& worldCoords, int& x, int& y) const
{
	m_matrixManager.WorldCoordsToWindowCoords(worldCoords, (float)viewport.GetX(), (float)viewport.GetY(), (float)viewport.GetWidth(), (float)viewport.GetHeight(), viewport.GetViewMatrix(), viewport.GetProjectionMatrix(), x, y);
}

void COpenGLRenderer::SetNumberOfLights(size_t count)
{
	static const string numberOfLightsKey = "lightsCount";
	int number = static_cast<int>(count);
	m_shaderManager.SetUniformValue(numberOfLightsKey, 1, 1, &number);
}

void COpenGLRenderer::SetUpLight(size_t index, CVector3f const& position, const float* ambient, const float* diffuse, const float* specular)
{
	const string key = "lights[" + to_string(index) + "].";
	m_shaderManager.SetUniformValue(key + "pos", 3, 1, position.ptr());
	m_shaderManager.SetUniformValue(key + "ambient", 4, 1, ambient);
	m_shaderManager.SetUniformValue(key + "diffuse", 4, 1, diffuse);
	m_shaderManager.SetUniformValue(key + "specular", 4, 1, specular);
}

float COpenGLRenderer::GetMaximumAnisotropyLevel() const
{
	float aniso = 1.0f;
	if (GLEW_EXT_texture_filter_anisotropic)
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	return aniso;
}

const float* COpenGLRenderer::GetProjectionMatrix() const
{
	return m_matrixManager.GetProjectionMatrix();
}

void COpenGLRenderer::EnableDepthTest(bool enable)
{
	if (enable)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void COpenGLRenderer::EnableBlending(bool enable)
{
	if (enable)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
}

void COpenGLRenderer::SetUpViewport(unsigned int viewportX, unsigned int viewportY, unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane, float farPane)
{
	m_matrixManager.SetUpViewport(viewportWidth, viewportHeight, viewingAngle, nearPane, farPane);
	glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
	m_viewport[0] = viewportX;
	m_viewport[1] = viewportY;
	m_viewport[2] = viewportWidth;
	m_viewport[3] = viewportHeight;
}

void COpenGLRenderer::EnablePolygonOffset(bool enable, float factor /*= 0.0f*/, float units /*= 0.0f*/)
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

void COpenGLRenderer::ClearBuffers(bool color, bool depth)
{
	GLbitfield mask = 0;
	if (color)
		mask |= GL_COLOR_BUFFER_BIT;
	if (depth)
		mask |= GL_DEPTH_BUFFER_BIT;
	glClear(mask);
}

void COpenGLRenderer::DrawIn2D(function<void()> const& drawHandler)
{
	m_matrixManager.SaveMatrices();
	m_matrixManager.SetOrthographicProjection(static_cast<float>(m_viewport[0]), static_cast<float>(m_viewport[2]), static_cast<float>(m_viewport[3]), static_cast<float>(m_viewport[1]));
	m_matrixManager.ResetModelView();

	drawHandler();

	m_matrixManager.RestoreMatrices();
}

void COpenGLRenderer::EnableMultisampling(bool enable)
{
	if (GLEW_ARB_multisample)
	{
		if (enable)
			glEnable(GL_MULTISAMPLE_ARB);
		else
			glDisable(GL_MULTISAMPLE_ARB);
	}
	else
	{
		throw runtime_error("MSAA is not supported");
	}
}

void COpenGLRenderer::BindVAO(unsigned vao, unsigned indexBuffer)
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
