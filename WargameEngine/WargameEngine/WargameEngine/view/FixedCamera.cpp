#include "FixedCamera.h"

CFixedCamera::CFixedCamera(CVector3d const& position, CVector3d const& direction, CVector3d const& up, double scale /*= 1.0*/)
	:m_position(position), m_direction(direction), m_up(up), m_scale(scale)
{
}

CVector3d CFixedCamera::GetPosition() const
{
	return m_position;
}

CVector3d CFixedCamera::GetDirection() const
{
	return m_direction;
}

CVector3d CFixedCamera::GetUpVector() const
{
	return m_up;
}

const double CFixedCamera::GetScale() const
{
	return m_scale;
}

void CFixedCamera::SetInput(IInput &)
{
}

void CFixedCamera::EnableTouchMode()
{
}
