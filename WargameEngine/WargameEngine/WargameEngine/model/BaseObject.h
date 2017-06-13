#pragma once
#include "..\Signal.h"
#include "IObject.h"
#include <float.h>

namespace wargameEngine
{
namespace model
{
template<class T>
class BaseObject : public T
{
public:
	BaseObject(const Path& model, const CVector3f& position, float rotation, bool hasShadow = true)
		: m_coords(position)
		, m_rotations(0.0f, 0.0f, rotation)
		, m_model(model)
		, m_castsShadow(hasShadow)
	{
	}

	Path GetPathToModel() const final
	{
		return m_model;
	}

	float GetX() const final
	{
		return m_coords.x;
	}

	float GetY() const final
	{
		return m_coords.y;
	}

	float GetZ() const final
	{
		return m_coords.z;
	}

	CVector3f GetCoords() const final
	{
		return m_coords;
	}

	float GetRotation() const final
	{
		return m_rotations.z;
	}

	bool CastsShadow() const final
	{
		return m_castsShadow;
	}

	void SetCoords(float x, float y, float z) final
	{
		SetCoords({ x, y, z });
	}

	void SetCoords(CVector3f const& coords) final
	{
		auto oldCoords = m_coords;
		m_coords = coords;
		if ((oldCoords - m_coords).GetLength() > FLT_EPSILON)
		{
			m_onCoordsChange(oldCoords, m_coords);
		}
	}

	void Rotate(float rotation) final
	{
		auto oldRotations = m_rotations;
		m_rotations.z = fmodf(m_rotations.z + rotation + 360.0f, 360.0f);
		if (fabsf(oldRotations.z - m_rotations.z) > FLT_EPSILON)
		{
			m_onRotationChange(oldRotations, m_rotations);
		}
	}

	void SetRotation(float rotation) final
	{
		auto oldRotations = m_rotations;
		m_rotations.z = rotation;
		if (fabsf(oldRotations.z - m_rotations.z) > FLT_EPSILON)
		{
			m_onRotationChange(oldRotations, m_rotations);
		}
	}

	CVector3f GetRotations() const override
	{
		return m_rotations;
	}

	void SetRotations(const CVector3f& rotations) final
	{
		if (m_rotations != rotations)
		{
			auto oldRotation = m_rotations;
			m_rotations = rotations;
			m_onRotationChange(oldRotation, m_rotations);
		}
	}

	void Move(float dx, float dy, float dz) final
	{
		auto oldCoords = m_coords;
		m_coords += { dx, dy, dz };
		if ((oldCoords - m_coords).GetLength() > FLT_EPSILON)
		{
			m_onCoordsChange(oldCoords, m_coords);
		}
	}

	IObject* GetFullObject() override
	{
		return nullptr;
	}

	signals::SignalConnection DoOnCoordsChange(typename T::CoordsSignal::Slot const& handler) override
	{
		return m_onCoordsChange.Connect(handler);
	}

	signals::SignalConnection DoOnRotationChange(typename T::RotationSignal::Slot const& handler) override
	{
		return m_onRotationChange.Connect(handler);
	}

private:
	CVector3f m_coords;
	CVector3f m_rotations;
	Path m_model;
	bool m_castsShadow;
	typename T::CoordsSignal m_onCoordsChange;
	typename T::RotationSignal m_onRotationChange;
};

using StaticObject = BaseObject<IBaseObject>;
}
}