#include "MirrorCamera.h"

CCameraMirror::CCameraMirror(ICamera * parentCamera, CVector3d const& translation /*= CVector3d()*/)
	:m_parentCamera(parentCamera), m_translation(translation)
{

}

CVector3d CCameraMirror::GetPosition() const
{
	return m_parentCamera->GetPosition() + m_translation;
}

CVector3d CCameraMirror::GetDirection() const
{
	return m_parentCamera->GetDirection() + m_translation;
}

CVector3d CCameraMirror::GetUpVector() const
{
	return m_parentCamera->GetUpVector();
}

const double CCameraMirror::GetScale() const
{
	return m_parentCamera->GetScale();
}

void CCameraMirror::SetInput(IInput &)
{
	//mirror camera has no inputs on its own
}

void CCameraMirror::EnableTouchMode()
{
	//mirror camera has no inputs on its own
}
