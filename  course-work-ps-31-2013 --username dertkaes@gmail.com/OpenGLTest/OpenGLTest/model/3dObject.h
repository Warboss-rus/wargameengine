#pragma once
#include "ObjectInterface.h"
#include <string>
#include <map>
#include <memory>
#include "..\view\Vector3.h"

class C3DObject : public IObject
{
public:
	C3DObject(std::string const& model, float x, float y, float rotation);
	std::string GetPathToModel() const { return m_model; }
	void Move(float x, float y, float z);
	void SetCoords(float x, float y, float z);
	void C3DObject::SetCoords(float3 pos);
	void Rotate(float rotation);
	float GetX() const { return m_x; }
	float GetY() const { return m_y; }
	float GetZ() const { return m_z; }
	CVector3d GetCoords() const { return CVector3d(m_x, m_y, m_z); }
	float GetRotation() const { return m_rotation; }
	std::set<std::string> const& GetHiddenMeshes() const { return m_hiddenMeshes; }
	void HideMesh(std::string const& meshName);
	void ShowMesh(std::string const& meshName);
	void SetProperty(std::string const& key, std::string const& value);
	std::string const GetProperty(std::string const& key) const;
	bool IsSelectable() const { return m_isSelectable; }
	void SetSelectable(bool selectable) { m_isSelectable = selectable; }
	void SetMovementLimiter(IMoveLimiter * limiter) { m_movelimiter.reset(limiter); }
private:
	std::string m_model;
	float m_x;
	float m_y;
	float m_z;
	float m_rotation;
	std::set<std::string> m_hiddenMeshes;
	std::map<std::string, std::string> m_properties;
	bool m_isSelectable;
	std::shared_ptr<IMoveLimiter> m_movelimiter;
};