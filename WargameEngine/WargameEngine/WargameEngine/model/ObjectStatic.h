#pragma once
#include "ObjectInterface.h"

template<class T>
class CBaseObject : public T
{
public:
	CBaseObject(std::wstring const& model, CVector3f const& position, float rotation, bool hasShadow = true)
		:m_coords(position), m_rotation(rotation), m_model(model), m_castsShadow(hasShadow) {}
	virtual std::wstring GetPathToModel() const final { return m_model; }
	virtual float GetX() const final { return m_coords.x; }
	virtual float GetY() const final { return m_coords.y; }
	virtual float GetZ() const final { return m_coords.z; }
	virtual CVector3f GetCoords() const final { return m_coords; }
	virtual float GetRotation() const final { return m_rotation; }
	virtual bool CastsShadow() const final { return m_castsShadow; }
	virtual void SetCoords(float x, float y, float z) final { m_coords = { x, y, z }; }
	virtual void SetCoords(CVector3f const& coords) final { m_coords = coords; }
	virtual void Rotate(float rotation) final { m_rotation = fmodf(m_rotation + rotation + 360.0f, 360.0f); }
	virtual void SetRotation(float rotation) final { m_rotation = rotation; }
	virtual void Move(float dx, float dy, float dz) final { m_coords += {dx, dy, dz}; }
protected:
	CVector3f m_coords;
	float m_rotation;
private:
	std::wstring m_model;
	bool m_castsShadow;
};

typedef CBaseObject<IBaseObject> CStaticObject;