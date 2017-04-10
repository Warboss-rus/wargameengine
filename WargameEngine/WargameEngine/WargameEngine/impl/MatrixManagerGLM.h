#pragma once
#include <vector>
#pragma warning(push)
#pragma warning(disable: 4201)
#include <glm/mat4x4.hpp>
#pragma warning(pop)
#include "../view/Vector3.h"

class IShaderManager;

class CMatrixManagerGLM
{
public:
	CMatrixManagerGLM();

	void InvalidateMatrices() const;
	void PushMatrix();
	void PopMatrix();
	void Translate(float dx, float dy, float dz);
	void Scale(float scale);
	void Rotate(float angle, float x, float y, float z);
	void GetModelViewMatrix(float * matrix) const;
	void GetProjectionMatrix(float * matrix) const;
	void GetViewMatrix(float * matrix) const;
	void ResetModelView();
	void LookAt(CVector3f const& position, CVector3f const& direction, CVector3f const& up, bool leftHanded = false);
	void WindowCoordsToWorldVector(int x, int y, float viewportX, float viewportY, float viewportWidth, float viewportHeight, const float * viewMatrix, const float * projectionMatrix, CVector3f & start, CVector3f & end) const;
	void WorldCoordsToWindowCoords(CVector3f const& worldCoords, float viewportX, float viewportY, float viewportWidth, float viewportHeight, const float * viewMatrix, const float * projectionMatrix, int& x, int& y) const;
	void SetUpViewport(unsigned int viewportWidth, unsigned int viewportHeight, float viewingAngle, float nearPane, float farPane, bool leftHanded = false);
	void UpdateMatrices(IShaderManager & shaderManager) const;
	void SaveMatrices();
	void RestoreMatrices();
	void SetOrthographicProjection(float left, float right, float bottom, float top);
	void SetProjectionMatrix(const float * matrix);
	void SetVrViewMatrices(std::vector<const float*> const& matrices);
private:
	std::vector<glm::mat4> m_modelMatrices;
	glm::mat4* m_modelMatrix;
	glm::mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;
	mutable bool m_matricesChanged = true;
	glm::mat4 m_savedViewMatrix;
	glm::mat4 m_savedProjectionMatrix;
	std::vector<glm::mat4> m_vrViewMatrices;
	bool m_2dMode = false;
};