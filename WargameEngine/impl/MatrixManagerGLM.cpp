#include "MatrixManagerGLM.h"
#include <glm/gtc/matrix_transform.hpp>
#pragma warning(push)
#pragma warning(disable: 4201)
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)
#include <glm/gtx/euler_angles.hpp>
#include "../view/IShaderManager.h"
#include <float.h>
#include <algorithm>

CMatrixManagerGLM::CMatrixManagerGLM()
{
	m_modelMatrices.push_back(glm::mat4());
	m_modelMatrix = &m_modelMatrices.back();
}

void CMatrixManagerGLM::InvalidateMatrices() const
{
	m_modelMatrixChanged = true;
	m_viewMatrixChanged = true;
	m_projectionMatrixChanged = true;
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
	m_modelMatrixChanged = true;
}

void CMatrixManagerGLM::Translate(float dx, float dy, float dz)
{
	if (fabs(dx) < FLT_EPSILON && fabs(dy) < FLT_EPSILON && fabs(dz) < FLT_EPSILON) return;
	*m_modelMatrix = glm::translate(*m_modelMatrix, glm::vec3(dx, dy, dz));
	m_modelMatrixChanged = true;
}

void CMatrixManagerGLM::Scale(float scale)
{
	if (fabs(scale - 1.0f) < FLT_EPSILON) return;
	*m_modelMatrix = glm::scale(*m_modelMatrix, glm::vec3(scale, scale, scale));
	m_modelMatrixChanged = true;
}

void CMatrixManagerGLM::Rotate(float angle, const CVector3f& axis)
{
	if (fabs(angle) < FLT_EPSILON) return;
	*m_modelMatrix = glm::rotate(*m_modelMatrix, glm::radians(angle), glm::make_vec3(axis.ptr()));
	m_modelMatrixChanged = true;
}


void CMatrixManagerGLM::Rotate(const CVector3f& rotations)
{
	if ((fabs(rotations.x) < FLT_EPSILON) && (fabs(rotations.y) < FLT_EPSILON) && (fabs(rotations.z) < FLT_EPSILON)) return;
	*m_modelMatrix *= glm::yawPitchRoll(glm::radians(rotations.x), glm::radians(rotations.y), glm::radians(rotations.z));
	m_modelMatrixChanged = true;
}

const float* CMatrixManagerGLM::GetModelMatrix() const
{
	return glm::value_ptr(*m_modelMatrix);
}

const float* CMatrixManagerGLM::GetProjectionMatrix() const
{
	return glm::value_ptr(m_projectionMatrix);
}

const float* CMatrixManagerGLM::GetViewMatrix() const
{
	return glm::value_ptr(m_viewMatrix);
}

void CMatrixManagerGLM::SetModelMatrix(const float* matrix)
{
	*m_modelMatrix = glm::make_mat4(matrix);
	m_modelMatrixChanged = true;
}

void CMatrixManagerGLM::ResetModelView()
{
	*m_modelMatrix = glm::mat4();
	m_viewMatrix = glm::mat4();
	m_modelMatrixChanged = true;
	m_viewMatrixChanged = true;
}

void CMatrixManagerGLM::LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up, bool leftHanded)
{
	if (leftHanded)
	{
		m_viewMatrix = glm::lookAtLH(glm::make_vec3(position.ptr()), glm::make_vec3(direction.ptr()), glm::make_vec3(up.ptr()));
	}
	else
	{
		m_viewMatrix = glm::lookAtRH(glm::make_vec3(position.ptr()), glm::make_vec3(direction.ptr()), glm::make_vec3(up.ptr()));
	}
	*m_modelMatrix = glm::mat4();
	m_modelMatrixChanged = true;
	m_viewMatrixChanged = true;
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

void CMatrixManagerGLM::SetUpViewport(unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane, float farPane, bool leftHanded)
{
	if (leftHanded)
	{
		m_projectionMatrix = glm::perspectiveFovLH<float>(glm::radians(viewingAngle), static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), nearPane, farPane);
	}
	else
	{
		m_projectionMatrix = glm::perspectiveFovRH<float>(glm::radians(viewingAngle), static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), nearPane, farPane);
	}
	m_projectionMatrixChanged = true;
}

void CMatrixManagerGLM::UpdateMatrices(wargameEngine::view::IShaderManager & shaderManager) const
{
	if (!m_modelMatrixChanged && !m_viewMatrixChanged && !m_projectionMatrixChanged) return;
	const float* model = m_modelMatrixChanged ? glm::value_ptr(*m_modelMatrix) : nullptr;
	const float* projection = m_projectionMatrixChanged ? glm::value_ptr(m_projectionMatrix) : nullptr;
	const float* view = nullptr;
	const float* mvp = nullptr;
	if (m_vrViewMatrices.empty() || m_disableVRMode)
	{
		view = m_viewMatrixChanged ? glm::value_ptr(m_viewMatrix) : nullptr;
		glm::mat4 mvpMat;
		if (shaderManager.NeedsMVPMatrix())
		{
			if (m_projectionMatrixChanged || m_viewMatrixChanged)
			{
				m_vpMatrix = m_projectionMatrix * m_viewMatrix;
			}
			mvpMat = m_vpMatrix * *m_modelMatrix;
			mvp = glm::value_ptr(mvpMat);
		}
		shaderManager.SetMatrices(model, view, projection, mvp, 1);
	}
	else
	{
		const bool updateMvp = shaderManager.NeedsMVPMatrix();
		static std::vector<glm::mat4> mvpMatrices;
		static std::vector<glm::mat4> viewMatrices;
		mvpMatrices.clear();
		viewMatrices.clear();
		for (auto& mat : m_vrViewMatrices)
		{
			viewMatrices.push_back(m_2dMode ? m_viewMatrix : m_viewMatrix * mat);
			if(updateMvp) mvpMatrices.push_back(m_projectionMatrix * viewMatrices.back() * *m_modelMatrix);
		}
		view = !viewMatrices.empty() ? glm::value_ptr(viewMatrices.front()) : nullptr;
		mvp = !mvpMatrices.empty() ? glm::value_ptr(mvpMatrices.front()) : nullptr;
		shaderManager.SetMatrices(model, view, projection, mvp, std::max<size_t>({ viewMatrices.size(), mvpMatrices.size(), 1 }));
	}
	m_modelMatrixChanged = false;
	m_viewMatrixChanged = false;
	m_projectionMatrixChanged = false;
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
	m_2dMode = false;
}

void CMatrixManagerGLM::SetOrthographicProjection(float left, float right, float bottom, float top)
{
	m_projectionMatrix = glm::ortho(left, right, bottom, top);
	m_projectionMatrixChanged = true;
	m_2dMode = true;
}

void CMatrixManagerGLM::SetProjectionMatrix(const float * matrix)
{
	m_projectionMatrix = glm::make_mat4(matrix);
	m_projectionMatrixChanged = true;
}

void CMatrixManagerGLM::SetVrViewMatrices(std::vector<const float*> const& matrices)
{
	m_vrViewMatrices.clear();
	for (auto& mat : matrices)
	{
		m_vrViewMatrices.push_back(glm::make_mat4(mat));
	}
}

void CMatrixManagerGLM::DisableVRMode(bool disable)
{
	m_disableVRMode = disable;
}
