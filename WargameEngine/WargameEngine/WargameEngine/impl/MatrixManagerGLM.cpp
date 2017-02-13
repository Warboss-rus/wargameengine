#include "MatrixManagerGLM.h"
#include <glm/gtc/matrix_transform.hpp>
#pragma warning(push)
#pragma warning(disable: 4201)
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)
#define _USE_MATH_DEFINES
#include <math.h>
#include "../view/IShaderManager.h"
#include <float.h>

CMatrixManagerGLM::CMatrixManagerGLM()
{
	m_modelMatrices.push_back(glm::mat4());
	m_modelMatrix = &m_modelMatrices.back();
}

void CMatrixManagerGLM::InvalidateMatrices() const
{
	m_matricesChanged = true;
}

void CMatrixManagerGLM::PushMatrix()
{
	m_modelMatrices.push_back(m_modelMatrices.back());
	m_modelMatrix = &m_modelMatrices.back();
}

void CMatrixManagerGLM::PopMatrix()
{
	m_modelMatrices.pop_back();
	m_modelMatrix = &m_modelMatrices.back();
	m_matricesChanged = true;
}

void CMatrixManagerGLM::Translate(float dx, float dy, float dz)
{
	if (fabs(dx) < FLT_EPSILON && fabs(dy) < FLT_EPSILON && fabs(dz) < FLT_EPSILON) return;
	*m_modelMatrix = glm::translate(*m_modelMatrix, glm::vec3(dx, dy, dz));
	m_matricesChanged = true;
}

void CMatrixManagerGLM::Scale(float scale)
{
	if (fabs(scale - 1.0f) < FLT_EPSILON) return;
	*m_modelMatrix = glm::scale(*m_modelMatrix, glm::vec3(scale, scale, scale));
	m_matricesChanged = true;
}

void CMatrixManagerGLM::Rotate(double angle, float x, float y, float z)
{
	if (fabs(angle) < DBL_EPSILON) return;
	*m_modelMatrix = glm::rotate(*m_modelMatrix, static_cast<float>(angle * M_PI / 180), glm::vec3(x, y, z));
	m_matricesChanged = true;
}

void CMatrixManagerGLM::GetModelViewMatrix(float * matrix) const
{
	glm::mat4 modelView = m_viewMatrix * *m_modelMatrix;
	memcpy(matrix, glm::value_ptr(modelView), sizeof(float) * 16);
}

void CMatrixManagerGLM::GetProjectionMatrix(float * matrix) const
{
	memcpy(matrix, glm::value_ptr(m_projectionMatrix), sizeof(float) * 16);
}

void CMatrixManagerGLM::ResetModelView()
{
	*m_modelMatrix = glm::mat4();
	m_viewMatrix = glm::mat4();
	m_matricesChanged = true;
}

void CMatrixManagerGLM::LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up)
{
	m_viewMatrix = glm::lookAt(glm::make_vec3(position.ptr()), glm::make_vec3(direction.ptr()), glm::make_vec3(up.ptr()));
	*m_modelMatrix = glm::mat4();
	m_matricesChanged = true;
}

void CMatrixManagerGLM::WindowCoordsToWorldVector(int x, int y, float viewportX, float viewportY, float viewportWidth, float viewportHeight, const float * viewMatrix, const float * projectionMatrix, CVector3f & start, CVector3f & end) const
{
	glm::vec4 viewportData(viewportX, viewportY, viewportWidth, viewportHeight);
	//Set OpenGL Windows coordinates
	float winX = (float)x;
	float winY = viewportData[3] - (float)y;

	auto ToVector3f = [](glm::vec3 const& v)->CVector3f { return{ v.x, v.y, v.z }; };
	//Cast a ray from eye to mouse cursor;
	glm::mat4 proj = glm::make_mat4(projectionMatrix);
	glm::mat4 view = glm::make_mat4(viewMatrix);
	start = ToVector3f(glm::unProject(glm::vec3(winX, winY, 0.0f), view, proj, viewportData));
	end = ToVector3f(glm::unProject(glm::vec3(winX, winY, 1.0f), view, proj, viewportData));
}

void CMatrixManagerGLM::WorldCoordsToWindowCoords(CVector3f const& worldCoords, float viewportX, float viewportY, float viewportWidth, float viewportHeight, const float * viewMatrix, const float * projectionMatrix, int& x, int& y) const
{
	glm::vec4 viewportData(viewportX , viewportY, viewportWidth, viewportHeight);
	auto windowPos = glm::project(glm::make_vec3(worldCoords.ptr()), glm::make_mat4(viewMatrix), glm::make_mat4(projectionMatrix), viewportData);
	x = static_cast<int>(windowPos.x);
	y = static_cast<int>(viewportData[3] - windowPos.y);
}

void CMatrixManagerGLM::SetUpViewport(unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane, float farPane)
{
	m_projectionMatrix = glm::perspectiveFov<float>(static_cast<float>(viewingAngle * 180.0 / M_PI), static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), nearPane, farPane);
	m_matricesChanged = true;
}

void CMatrixManagerGLM::UpdateMatrices(IShaderManager & shaderManager) const
{
	if (!m_matricesChanged) return;
	glm::mat4 m = m_projectionMatrix * m_viewMatrix * *m_modelMatrix;
	static const std::string mvpMatrixKey = "mvp_matrix";
	static const std::string view_matrix_key = "view_matrix";
	static const std::string model_matrix_key = "model_matrix";
	static const std::string proj_matrix_key = "proj_matrix";
	shaderManager.SetUniformValue(mvpMatrixKey, 16, 1, glm::value_ptr(m));
	shaderManager.SetUniformValue(view_matrix_key, 16, 1, glm::value_ptr(m_viewMatrix));
	shaderManager.SetUniformValue(model_matrix_key, 16, 1, glm::value_ptr(*m_modelMatrix));
	shaderManager.SetUniformValue(proj_matrix_key, 16, 1, glm::value_ptr(m_projectionMatrix));
	m_matricesChanged = false;
}

void CMatrixManagerGLM::SaveMatrices()
{
	m_savedProjectionMatrix = m_projectionMatrix;
	m_savedViewMatrix = m_viewMatrix;
	PushMatrix();
}

void CMatrixManagerGLM::RestoreMatrices()
{
	m_projectionMatrix = m_savedProjectionMatrix;
	m_viewMatrix = m_savedViewMatrix;
	PopMatrix();
}

void CMatrixManagerGLM::SetOrthographicProjection(float left, float right, float bottom, float top)
{
	m_projectionMatrix = glm::ortho(left, right, bottom, top);
	m_matricesChanged = true;
}
