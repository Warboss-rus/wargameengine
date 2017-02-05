#pragma once
#include "ObjectInterface.h"
#include "..\Signal.h"

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
	virtual void SetCoords(float x, float y, float z) final { SetCoords({ x, y, z }); }
	virtual void SetCoords(CVector3f const& coords) final 
	{ 
		auto oldCoords = m_coords;
		m_coords = coords;
		if ((oldCoords - m_coords).GetLength() > FLT_EPSILON)
		{
			m_onCoordsChange(oldCoords, m_coords);
		}
	}
	virtual void Rotate(float rotation) final 
	{ 
		auto oldRotation = m_rotation;  
		m_rotation = fmodf(m_rotation + rotation + 360.0f, 360.0f);
		if (abs(oldRotation - m_rotation) > FLT_EPSILON)
		{
			m_onRotationChange(oldRotation, m_rotation);
		}
	}
	virtual void SetRotation(float rotation) final
	{ 
		auto oldRotation = m_rotation;
		m_rotation = rotation;
		if (abs(oldRotation - m_rotation) > FLT_EPSILON)
		{
			m_onRotationChange(oldRotation, m_rotation);
		}
	}
	virtual void Move(float dx, float dy, float dz) final 
	{ 
		auto oldCoords = m_coords;
		m_coords += {dx, dy, dz}; 
		if ((oldCoords - m_coords).GetLength() > FLT_EPSILON)
		{
			m_onCoordsChange(oldCoords, m_coords);
		}
	}
	virtual IObject* GetFullObject() override { return nullptr; }
	virtual typename CSignalConnection DoOnCoordsChange(typename T::CoordsSignal::Slot const& handler) override { return std::move(m_onCoordsChange.Connect(handler)); }
	virtual typename CSignalConnection DoOnRotationChange(typename T::RotationSignal::Slot const& handler) override { return std::move(m_onRotationChange.Connect(handler)); }
private:
	CVector3f m_coords;
	float m_rotation;
	std::wstring m_model;
	bool m_castsShadow;
	typename T::CoordsSignal m_onCoordsChange;
	typename T::RotationSignal m_onRotationChange;
};

typedef CBaseObject<IBaseObject> CStaticObject;