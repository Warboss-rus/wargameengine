#include "FixedCamera.h"

CFixedCamera::CFixedCamera(CVector3f const& position, CVector3f const& direction, CVector3f const& up, float scale /*= 1.0*/)
	:m_position(position), m_direction(direction), m_up(up), m_scale(scale)
{
}

CVector3f CFixedCamera::GetPosition() const
{
	return m_position;
}

CVector3f CFixedCamera::GetDirection() const
{
	return m_direction;
}

CVector3f CFixedCamera::GetUpVector() const
{
	return m_up;
}

const float CFixedCamera::GetScale() const
{
	return m_scale;
}

void CFixedCamera::SetInput(IInput &)
{
}

void CFixedCamera::EnableTouchMode()
{
}
