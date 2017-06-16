#include "LegacyOpenGLRenderer.h"
#include <GL/glew.h>
#include "gl.h"
#include "../LogWriter.h"
#include "../view/TextureManager.h"
#include "ShaderManagerOpenGL.h"
#include "../view/Matrix4.h"
#include "../view/IViewport.h"

using namespace std;
using namespace wargameEngine;
using namespace view;

class CLegacyGLVertexBuffer : public IVertexBuffer
{
public:
	CLegacyGLVertexBuffer(const float * vertex = nullptr, const float * normals = nullptr, const float * texcoords = nullptr, size_t size = 0, bool temp = true);
	~CLegacyGLVertexBuffer();
	void Bind() const;
	void SetIndexBuffer(const unsigned int * indexPtr, size_t indexesSize);
	void DrawIndexed(size_t begin, size_t count, size_t instances) const
	{
		if (instances > 1)
		{
			glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, m_indexesBuffer ? reinterpret_cast<void*>(begin * sizeof(unsigned int)) : m_indexes + begin, instances);
		}
		else
		{
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, m_indexesBuffer ? reinterpret_cast<void*>(begin * sizeof(unsigned int)) : m_indexes + begin);
		}
	}

private:
	const float * m_vertex = NULL;
	const float * m_normals = NULL;
	const float * m_texCoords = NULL;
	const unsigned int* m_indexes = NULL;
	GLuint m_vertexBuffer = NULL;
	GLuint m_normalsBuffer = NULL;
	GLuint m_texCoordBuffer = NULL;
	GLuint m_indexesBuffer = NULL;
};

class CLegacyGLFrameBuffer : public IFrameBuffer
{
public:
	CLegacyGLFrameBuffer();
	~CLegacyGLFrameBuffer();
	virtual void Bind() const override;
	virtual void UnBind() const override;
	virtual void AssignTexture(ICachedTexture & texture, IRenderer::CachedTextureType type) override;
private:
	unsigned int m_id;
};

class CLegacyGLOcclusionQuery : public IOcclusionQuery
{
public:
	CLegacyGLOcclusionQuery()
	{
		glGenQueries(1, &m_id);
	}
	~CLegacyGLOcclusionQuery()
	{
		glDeleteQueries(1, &m_id);
	}
	virtual void Query(std::function<void() > const& handler) override
	{
		glBeginQuery(GL_ANY_SAMPLES_PASSED, m_id);
		handler();
		glEndQuery(GL_ANY_SAMPLES_PASSED);
	}

	virtual bool IsVisible() const override
	{
		if (GLEW_ARB_query_buffer_object)
		{
			int result = 1;//true by default
			glGetQueryObjectiv(m_id, GL_QUERY_RESULT_NO_WAIT, &result);
			return result != 0;
		}
		else
		{
			GLint result = 0;
			glGetQueryObjectiv(m_id, GL_QUERY_RESULT_AVAILABLE, &result);
			int err = glGetError();
			if (result != 0)
			{
				glGetQueryObjectiv(m_id, GL_QUERY_RESULT, &result);
				err = glGetError();
				return result != 0;
			}
			return true;
		}
	}
private:
	GLuint m_id = 0;
};

class CLegacyGlCachedTexture : public ICachedTexture
{
public:
	CLegacyGlCachedTexture(unsigned int type);
	~CLegacyGlCachedTexture();

	operator unsigned int() const { return m_id; }
	unsigned GetType() const { return m_type; }
private:
	unsigned int m_id;
	unsigned int m_type;
};

void CLegacyGLRenderer::SetTexture(const Path& texture, bool forceLoadNow, int flags)
{
	if (forceLoadNow)
	{
		m_textureManager->LoadTextureNow(texture, flags);
	}
	SetTexture(*m_textureManager->GetTexturePtr(texture, nullptr, flags));
}

void CLegacyGLRenderer::SetTexture(ICachedTexture const& texture, TextureSlot slot /*= TextureSlot::eDiffuse*/)
{
	if (slot != TextureSlot::Diffuse) glActiveTexture(GL_TEXTURE0 + static_cast<int>(slot));
	auto& glTexture = reinterpret_cast<CLegacyGlCachedTexture const&>(texture);
	glBindTexture(glTexture.GetType(), glTexture);
	if (slot != TextureSlot::Diffuse) glActiveTexture(GL_TEXTURE0);
}

static const map<IRenderer::RenderMode, GLenum> renderModeMap = {
	{ IRenderer::RenderMode::Triangles, GL_TRIANGLES },
	{ IRenderer::RenderMode::TriangleStrip, GL_TRIANGLE_STRIP },
	{ IRenderer::RenderMode::Lines, GL_LINES },
	{ IRenderer::RenderMode::LineLoop, GL_LINE_LOOP }
};

void Bind(const void* vertices, const void* normals, const void* texCoords, GLenum vertexType, GLenum normalType, GLenum texCoordType, int vertexAxesCount)
{
	if (vertices)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(vertexAxesCount, vertexType, 0, vertices);
	}
	else
	{
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	if (normals)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(normalType, 0, normals);
	}
	else
	{
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	if (texCoords)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, texCoordType, 0, texCoords);
	}
	else
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}

CLegacyGLRenderer::CLegacyGLRenderer()
	:m_textureManager(nullptr)
{
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.01f);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glewInit();
}

void CLegacyGLRenderer::RenderArrays(RenderMode mode, array_view<CVector3f> const& vertices, array_view<CVector3f> const& normals, array_view<CVector2f> const& texCoords)
{
	Bind(vertices.data(), normals.data(), texCoords.data(), GL_FLOAT, GL_FLOAT, GL_FLOAT, 3);
	glDrawArrays(renderModeMap.at(mode), 0, vertices.size());
}

void CLegacyGLRenderer::RenderArrays(RenderMode mode, array_view<CVector2i> const& vertices, array_view<CVector2f> const& texCoords)
{
	Bind(vertices.data(), NULL, texCoords.data(), GL_INT, GL_FLOAT, GL_FLOAT, 2);
	glDrawArrays(renderModeMap.at(mode), 0, vertices.size());
}

void CLegacyGLRenderer::PushMatrix()
{
	glPushMatrix();
}

void CLegacyGLRenderer::PopMatrix()
{
	glPopMatrix();
}

void CLegacyGLRenderer::Translate(int dx, int dy, int dz)
{
	glTranslated(static_cast<double>(dx), static_cast<double>(dy), static_cast<double>(dz));
}

void CLegacyGLRenderer::Translate(const CVector3f& delta)
{
	glTranslatef(delta.x, delta.y, delta.z);
}

void CLegacyGLRenderer::Scale(float scale)
{
	glScalef(scale, scale, scale);
}

void CLegacyGLRenderer::Rotate(float angle, const CVector3f& axis)
{
	glRotatef(angle, axis.x, axis.y, axis.z);
}

void CLegacyGLRenderer::Rotate(const CVector3f& rotations)
{
	glRotatef(rotations.x, 1.0f, 0.0f, 0.0f);
	glRotatef(rotations.y, 0.0f, 1.0f, 0.0f);
	glRotatef(rotations.z, 0.0f, 0.0f, 1.0f);
}

const float* CLegacyGLRenderer::GetViewMatrix() const
{
	glGetFloatv(GL_MODELVIEW_MATRIX, m_matrix);
	return m_matrix;
}

const float* CLegacyGLRenderer::GetModelMatrix() const
{
	glGetFloatv(GL_MODELVIEW_MATRIX, m_matrix);
	return m_matrix;
}

void CLegacyGLRenderer::SetModelMatrix(const float* matrix)
{
	glLoadIdentity();
	glMultMatrixf(matrix);
}

void CLegacyGLRenderer::ResetViewMatrix()
{
	glLoadIdentity();
}

void CLegacyGLRenderer::LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up)
{
	gluLookAt(position[0], position[1], position[2], direction[0], direction[1], direction[2], up[0], up[1], up[2]);
}

void CLegacyGLRenderer::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	glColor4ub(r, g, b, a);
}

void CLegacyGLRenderer::SetColor(const float * color)
{
	glColor4fv(color);
}

void CLegacyGLRenderer::RenderToTexture(std::function<void() > const& func, ICachedTexture & tex, unsigned int width, unsigned int height)
{
	//set up texture
	GLint prevTexture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
	auto texture = reinterpret_cast<CLegacyGlCachedTexture&>(tex);
	SetTexture(texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	SetTexture(L"");
	//set up buffer
	GLint prevBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevBuffer);
	GLuint framebuffer = 0;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		LogWriter::WriteLine("framebuffer error code=" + std::to_string(status));
	}
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, width, height);
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glScalef(1.0f, -1.0f, 1.0f);
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT);
	func();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();

	glBindFramebuffer(GL_FRAMEBUFFER, prevBuffer);
	glBindTexture(GL_TEXTURE_2D, prevTexture);
	glDeleteFramebuffers(1, &framebuffer);
}

std::unique_ptr<ICachedTexture> CLegacyGLRenderer::CreateTexture(const void * data, unsigned int width, unsigned int height, CachedTextureType type)
{
	static const std::map<CachedTextureType, GLenum> typeMap = {
		{ CachedTextureType::RGBA, GL_RGBA },
		{ CachedTextureType::RenderTarget, GL_RGBA },
		{ CachedTextureType::Alpha, GL_ALPHA },
		{ CachedTextureType::Depth, GL_DEPTH_COMPONENT }
	};
	auto texture = std::make_unique<CLegacyGlCachedTexture>(GL_TEXTURE_2D);
	SetTexture(*texture);
	glTexImage2D(GL_TEXTURE_2D, 0, typeMap.at(type), width, height, 0, typeMap.at(type), GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	if (type == CachedTextureType::Depth)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	}
	return move(texture);
}

std::unique_ptr<IVertexBuffer> CLegacyGLRenderer::CreateVertexBuffer(const float * vertex, const float * normals, const float * texcoords, size_t size, bool temp)
{
	return std::make_unique<CLegacyGLVertexBuffer>(vertex, normals, texcoords, size, temp);
}

std::unique_ptr<IFrameBuffer> CLegacyGLRenderer::CreateFramebuffer() const
{
	return std::make_unique<CLegacyGLFrameBuffer>();
}

IShaderManager& CLegacyGLRenderer::GetShaderManager()
{
	return m_shaderManager;
}

void CLegacyGLRenderer::SetTextureManager(TextureManager & textureManager)
{
	m_textureManager = &textureManager;
}

void CLegacyGLRenderer::SetMaterial(const float * ambient, const float * diffuse, const float * specular, float shininess)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

CLegacyGlCachedTexture::CLegacyGlCachedTexture(unsigned int type)
	:m_type(type)
{
	glGenTextures(1, &m_id);
}

CLegacyGlCachedTexture::~CLegacyGlCachedTexture()
{
	glDeleteTextures(1, &m_id);
}

CLegacyGLVertexBuffer::CLegacyGLVertexBuffer(const float * vertex, const float * normals, const float * texcoords, size_t size, bool temp)
{
	if (temp)
	{
		m_vertex = vertex;
		m_normals = normals;
		m_texCoords = texcoords;
	}
	else
	{
		if (vertex)
		{
			glGenBuffers(1, &m_vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, size * 3 * sizeof(float), vertex, GL_STATIC_DRAW);
		}
		if (normals)
		{
			glGenBuffers(1, &m_normalsBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_normalsBuffer);
			glBufferData(GL_ARRAY_BUFFER, size * 3 * sizeof(float), normals, GL_STATIC_DRAW);
		}
		if (texcoords)
		{
			glGenBuffers(1, &m_texCoordBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
			glBufferData(GL_ARRAY_BUFFER, size * 2 * sizeof(float), texcoords, GL_STATIC_DRAW);
		}
		glBindBuffer(GL_ARRAY_BUFFER, NULL);
	}
}

CLegacyGLVertexBuffer::~CLegacyGLVertexBuffer()
{
	glDeleteBuffers(4, &m_vertexBuffer);
}

void CLegacyGLVertexBuffer::Bind() const
{
	if (m_vertex || m_vertexBuffer)
	{
		if (m_vertexBuffer) glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, m_vertexBuffer ? 0 : m_vertex);
	}
	if (m_normals || m_normalsBuffer)
	{
		if (m_normalsBuffer) glBindBuffer(GL_ARRAY_BUFFER, m_normalsBuffer);
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, m_normalsBuffer ? 0 : m_normals);
	}
	if (m_texCoords || m_texCoordBuffer)
	{
		if (m_texCoordBuffer) glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, m_texCoordBuffer ? 0 : m_texCoords);
	}
	if (m_indexesBuffer)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexesBuffer);
	}
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

void CLegacyGLVertexBuffer::SetIndexBuffer(const unsigned int * indexPtr, size_t indexesSize)
{
	if ((m_vertexBuffer || m_normalsBuffer || m_texCoordBuffer) && indexPtr)
	{
		glGenBuffers(1, &m_indexesBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexesBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexesSize * sizeof(unsigned), indexPtr, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
	}
	else
	{
		m_indexes = indexPtr;
	}
}

void CLegacyGLRenderer::DrawIndexed(IVertexBuffer& buffer, size_t begin, size_t count, size_t instances)
{
	auto& glBuffer = reinterpret_cast<const CLegacyGLVertexBuffer&>(buffer);
	glBuffer.Bind();
	glBuffer.DrawIndexed(begin, count, instances);
}

void CLegacyGLRenderer::Draw(IVertexBuffer& buffer, size_t count, size_t begin, size_t instances)
{
	reinterpret_cast<const CLegacyGLVertexBuffer&>(buffer).Bind();
	if (instances > 1)
	{
		glDrawArraysInstanced(GL_TRIANGLES, begin, count, instances);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, begin, count);
	}
}

typedef struct {
	unsigned count;
	unsigned primCount;
	unsigned firstIndex;
	unsigned baseVertex;
	unsigned baseInstance;
} DrawElementsIndirectCommand;

typedef  struct {
	GLuint  count;
	GLuint  primCount;
	GLuint  first;
	GLuint  baseInstance;
} DrawArraysIndirectCommand;

void CLegacyGLRenderer::DrawIndirect(IVertexBuffer& buffer, const array_view<IndirectDraw>& indirectList, bool indexed)
{
	if (GL_ARB_draw_indirect && indirectList.size() > 5)
	{
		reinterpret_cast<CLegacyGLVertexBuffer&>(buffer).Bind();
		GLuint drawIndirectBuffer;
		glGenBuffers(1, &drawIndirectBuffer);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, drawIndirectBuffer);
		if (indexed)
		{
			std::vector<DrawElementsIndirectCommand> commands;
			std::transform(indirectList.begin(), indirectList.end(), std::back_inserter(commands), [](const IndirectDraw& indirect) {
				return DrawElementsIndirectCommand{ static_cast<GLuint>(indirect.count), static_cast<GLuint>(indirect.instances), static_cast<GLuint>(indirect.start), 0, 0 };
			});
			glBufferData(GL_DRAW_INDIRECT_BUFFER, commands.size() * sizeof(DrawElementsIndirectCommand), commands.data(), GL_STREAM_DRAW);
			if (GL_ARB_multi_draw_indirect)
			{
				glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL, indirectList.size(), sizeof(DrawElementsIndirectCommand));
			}
			else
			{
				for (size_t i = 0; i < indirectList.size(); ++i)
				{
					glDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(i * sizeof(DrawElementsIndirectCommand)));
				}
			}
		}
		else
		{
			std::vector<DrawArraysIndirectCommand> commands;
			std::transform(indirectList.begin(), indirectList.end(), std::back_inserter(commands), [](const IndirectDraw& indirect) {
				return DrawArraysIndirectCommand{ static_cast<GLuint>(indirect.count), static_cast<GLuint>(indirect.instances), static_cast<GLuint>(indirect.start), 0 };
			});
			glBufferData(GL_DRAW_INDIRECT_BUFFER, commands.size() * sizeof(DrawArraysIndirectCommand), commands.data(), GL_STREAM_DRAW);
			if (GL_ARB_multi_draw_indirect)
			{
				glMultiDrawArraysIndirect(GL_TRIANGLES, NULL, indirectList.size(), sizeof(DrawArraysIndirectCommand));
			}
			else
			{
				for (size_t i = 0; i < indirectList.size(); ++i)
				{
					glDrawArraysIndirect(GL_TRIANGLES, (void*)(i * sizeof(DrawArraysIndirectCommand)));
				}
			}
		}
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
		glDeleteBuffers(1, &drawIndirectBuffer);
	}
	else
	{
		for (auto& indirect : indirectList)
		{
			if (indexed)
			{
				DrawIndexed(buffer, indirect.count, indirect.start, indirect.instances);
			}
			else
			{
				Draw(buffer, indirect.count, indirect.start, indirect.instances);
			}
		}
	}
}

void CLegacyGLRenderer::SetIndexBuffer(IVertexBuffer& buffer, const unsigned int* indexPtr, size_t indexesSize)
{
	reinterpret_cast<CLegacyGLVertexBuffer&>(buffer).SetIndexBuffer(indexPtr, indexesSize);
}

void CLegacyGLRenderer::AddVertexAttribute(IVertexBuffer& buffer, const std::string& attribute, int elementSize, size_t count, IShaderManager::Format type, const void* values, bool perInstance /*= false*/)
{

}

std::vector<double> Matrix2DoubleArray(Matrix4F const& matrix)
{
	std::vector<double> result(16);
	for (size_t i = 0; i < 16; ++i)
	{
		result[i] = matrix[i];
	}
	return result;
}

void CLegacyGLRenderer::WindowCoordsToWorldVector(IViewport & viewport, int x, int y, CVector3f & start, CVector3f & end) const
{
	//Get model, projection and viewport matrices
	auto matModelView = Matrix2DoubleArray(viewport.GetViewMatrix());
	auto matProjection = Matrix2DoubleArray(viewport.GetProjectionMatrix());
	int viewportData[4] = {viewport.GetX(), viewport.GetY(), viewport.GetWidth(), viewport.GetHeight()};
	//Set OpenGL Windows coordinates
	double winX = (double)x;
	double winY = viewportData[3] - (double)y;

	CVector3d startd;
	CVector3d endd;
	//Cast a ray from eye to mouse cursor;
	gluUnProject(winX, winY, 0.0, matModelView.data(), matProjection.data(),
		viewportData, &startd.x, &startd.y, &startd.z);
	gluUnProject(winX, winY, 1.0, matModelView.data(), matProjection.data(),
		viewportData, &endd.x, &endd.y, &endd.z);
	start = { static_cast<float>(startd.x), static_cast<float>(startd.y), static_cast<float>(startd.z) };
	end = { static_cast<float>(endd.x), static_cast<float>(endd.y), static_cast<float>(endd.z) };
}

void CLegacyGLRenderer::WorldCoordsToWindowCoords(IViewport & viewport, CVector3f const& worldCoords, int& x, int& y) const
{
	auto matModelView = Matrix2DoubleArray(viewport.GetViewMatrix());
	auto matProjection = Matrix2DoubleArray(viewport.GetProjectionMatrix());
	int viewportData[4] = { viewport.GetX(), viewport.GetY(), viewport.GetWidth(), viewport.GetHeight() };
	CVector3d windowPos;
	if (gluProject(worldCoords.x, worldCoords.y, worldCoords.z, matModelView.data(), matProjection.data(), viewportData, &windowPos.x, &windowPos.y, &windowPos.z) != GL_FALSE)
	{
		x = static_cast<int>(windowPos.x);
		y = static_cast<int>(viewportData[3] - windowPos.y);
	}
}

void CLegacyGLRenderer::SetNumberOfLights(size_t count)
{
	for (GLenum i = GL_LIGHT0; i <= GL_LIGHT7; ++i)
	{
		if (i - GL_LIGHT0 < count)
		{
			glEnable(i);
		}
		else
		{
			glDisable(i);
		}
	}
}

void CLegacyGLRenderer::SetUpLight(size_t index, CVector3f const& position, const float * ambient, const float * diffuse, const float * specular)
{
	glLightfv(GL_LIGHT0 + index, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0 + index, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0 + index, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0 + index, GL_POSITION, position);
}

float CLegacyGLRenderer::GetMaximumAnisotropyLevel() const
{
	float aniso = 1.0f;
	if (GLEW_EXT_texture_filter_anisotropic)
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	return aniso;
}

const float* CLegacyGLRenderer::GetProjectionMatrix() const
{
	glGetFloatv(GL_PROJECTION_MATRIX, m_matrix);
	return m_matrix;
}

void CLegacyGLRenderer::EnableDepthTest(bool read, bool write)
{
	if(read)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	glDepthMask(write ? GL_TRUE : GL_FALSE);
}

void CLegacyGLRenderer::EnableColorWrite(bool rgb, bool alpha)
{
	const GLboolean rgbMask = rgb ? GL_TRUE : GL_FALSE;
	glColorMask(rgbMask, rgbMask, rgbMask, alpha ? GL_TRUE : GL_FALSE);
}

void CLegacyGLRenderer::EnableBlending(bool enable)
{
	if (enable)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);
}

void CLegacyGLRenderer::SetUpViewport(unsigned int viewportX, unsigned int viewportY, unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane, float farPane)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLdouble aspect = (GLdouble)viewportWidth / (GLdouble)viewportHeight;
	gluPerspective(viewingAngle, aspect, nearPane, farPane);
	glMatrixMode(GL_MODELVIEW);
	glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
}

void CLegacyGLRenderer::EnablePolygonOffset(bool enable, float factor /*= 0.0f*/, float units /*= 0.0f*/)
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

void CLegacyGLRenderer::ClearBuffers(bool color, bool depth)
{
	GLbitfield mask = 0;
	if (color) mask |= GL_COLOR_BUFFER_BIT;
	if (depth) mask |= GL_DEPTH_BUFFER_BIT;
	glClear(mask);
}


void CLegacyGLRenderer::UnbindTexture(TextureSlot slot)
{
	if (slot != TextureSlot::Diffuse) glActiveTexture(GL_TEXTURE0 + static_cast<int>(slot));
	glBindTexture(GL_TEXTURE_2D, 0);
	if (slot != TextureSlot::Diffuse) glActiveTexture(GL_TEXTURE0);
}

std::unique_ptr<ICachedTexture> CLegacyGLRenderer::CreateEmptyTexture(bool cubemap)
{
	return std::make_unique<CLegacyGlCachedTexture>(cubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
}

void CLegacyGLRenderer::SetTextureAnisotropy(float value)
{
	if (GLEW_EXT_texture_filter_anisotropic)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);
	}
}

void CLegacyGLRenderer::UploadTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, unsigned short bpp, int flags, TextureMipMaps const& mipmaps)
{
	SetTexture(texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE_EXT : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE_EXT : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_BUILD_MIPMAPS || !mipmaps.empty()) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	GLenum format = (flags & TEXTURE_BGRA) ? ((flags & TEXTURE_HAS_ALPHA) ? GL_BGRA_EXT : GL_BGR_EXT) : ((flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB);
	if (flags & TEXTURE_BUILD_MIPMAPS)
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, bpp / 8, width, height, format, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, bpp / 8, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	}
	for (size_t i = 0; i < mipmaps.size(); i++)
	{
		auto& mipmap = mipmaps[i];
		glTexImage2D(GL_TEXTURE_2D, i + 1, bpp / 8, mipmap.width, mipmap.height, 0, format, GL_UNSIGNED_BYTE, mipmap.data);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmaps.size());
}

void CLegacyGLRenderer::UploadCompressedTexture(ICachedTexture & texture, unsigned char * data, size_t width, size_t height, size_t size, int flags, TextureMipMaps const& mipmaps)
{
	SetTexture(texture);
	if (!GLEW_EXT_texture_compression_s3tc)
	{
		LogWriter::WriteLine("Compressed textures are not supported");
		return;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE_EXT : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE_EXT : GL_REPEAT);
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
		glCompressedTexImage2DARB(GL_TEXTURE_2D, i + 1, format, mipmap.width, mipmap.height, 0, mipmap.size, mipmap.data);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmaps.size());
}

void CLegacyGLRenderer::UploadCubemap(ICachedTexture & texture, TextureMipMaps const& sides, unsigned short, int flags)
{
	SetTexture(texture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, (flags & TextureFlags::TEXTURE_NO_WRAP) ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, (flags & TEXTURE_BUILD_MIPMAPS) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	GLenum format = (flags & TEXTURE_BGRA) ? ((flags & TEXTURE_HAS_ALPHA) ? GL_BGRA : GL_BGR_EXT) : ((flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB);
	for (size_t i = 0; i < sides.size(); ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, (flags & TEXTURE_HAS_ALPHA) ? GL_RGBA : GL_RGB, static_cast<GLsizei>(sides[i].width), static_cast<GLsizei>(sides[i].height), 0, format, GL_UNSIGNED_BYTE, sides[i].data);
	}
	if (flags & TEXTURE_BUILD_MIPMAPS)
	{
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
}

bool CLegacyGLRenderer::Force32Bits() const
{
	return false;
}

bool CLegacyGLRenderer::ForceFlipBMP() const
{
	return false;
}

bool CLegacyGLRenderer::ConvertBgra() const
{
	return false;
}

std::string CLegacyGLRenderer::GetName() const
{
	return "OpenGL";
}

bool CLegacyGLRenderer::SupportsFeature(Feature feature) const
{
	if (feature == Feature::Instancing)
	{
		return GLEW_ARB_draw_instanced && GLEW_ARB_instanced_arrays;
	}
	return true;
}

void CLegacyGLRenderer::EnableMultisampling(bool enable)
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
		throw std::runtime_error("MSAA is not supported");
	}
}

std::unique_ptr<IOcclusionQuery> CLegacyGLRenderer::CreateOcclusionQuery()
{
	return std::make_unique<CLegacyGLOcclusionQuery>();
}

void CLegacyGLRenderer::DrawIn2D(std::function<void()> const& drawHandler)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glEnable(GL_BLEND);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(viewport[0], viewport[2], viewport[3], viewport[1], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	drawHandler();

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

CLegacyGLFrameBuffer::CLegacyGLFrameBuffer()
{
	if (!GLEW_EXT_framebuffer_object)
	{
		throw std::runtime_error("GL_EXT_framebuffer_object is not supported");
	}
	glGenFramebuffers(1, &m_id);
	Bind();
}

CLegacyGLFrameBuffer::~CLegacyGLFrameBuffer()
{
	UnBind();
	glDeleteBuffers(1, &m_id);
}

void CLegacyGLFrameBuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void CLegacyGLFrameBuffer::UnBind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}

void CLegacyGLFrameBuffer::AssignTexture(ICachedTexture & texture, IRenderer::CachedTextureType type)
{
	static const std::map<IRenderer::CachedTextureType, GLenum> typeMap = {
		{ IRenderer::CachedTextureType::RGBA, GL_COLOR_ATTACHMENT0 },
		{ IRenderer::CachedTextureType::RenderTarget, GL_COLOR_ATTACHMENT0 },
		{ IRenderer::CachedTextureType::Alpha, GL_STENCIL_ATTACHMENT },
		{ IRenderer::CachedTextureType::Depth, GL_DEPTH_ATTACHMENT }
	};
	const std::map<IRenderer::CachedTextureType, pair<GLboolean, string>> extensionMap = {
		{ IRenderer::CachedTextureType::RGBA, {GLEW_ARB_color_buffer_float, "GL_ARB_color_buffer_float" }},
		{ IRenderer::CachedTextureType::RenderTarget,{ GLEW_ARB_color_buffer_float, "GL_ARB_color_buffer_float" } },
		{ IRenderer::CachedTextureType::Alpha, {GLEW_ARB_stencil_texturing, "GL_ARB_stencil_texturing" }},
		{ IRenderer::CachedTextureType::Depth, {GLEW_ARB_depth_buffer_float, "GL_ARB_depth_buffer_float" }}
	};
	if (!extensionMap.at(type).first)
	{
		throw std::runtime_error(extensionMap.at(type).second + " is not supported");
	}
	if (type == IRenderer::CachedTextureType::Depth)
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, typeMap.at(type), GL_TEXTURE_2D, (CLegacyGlCachedTexture&)texture, 0);
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("Error creating framebuffer");
	}
}