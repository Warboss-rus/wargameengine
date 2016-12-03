#include "MirrorCamera.h"

CCameraMirror::CCameraMirror(ICamera * parentCamera, CVector3f const& translation /*= CVector3d()*/)
	:m_parentCamera(parentCamera), m_translation(translation)
{

}

CVector3f CCameraMirror::GetPosition() const
{
	return m_parentCamera->GetPosition() + m_translation;
}

CVector3f CCameraMirror::GetDirection() const
{
	return m_parentCamera->GetDirection() + m_translation;
}

CVector3f CCameraMirror::GetUpVector() const
{
	return m_parentCamera->GetUpVector();
}

const float CCameraMirror::GetScale() const
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
