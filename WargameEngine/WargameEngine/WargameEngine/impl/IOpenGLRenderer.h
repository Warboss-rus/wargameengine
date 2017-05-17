#pragma once
#include "..\view\IViewHelper.h"

class IOpenGLRenderer : public wargameEngine::view::IViewHelper
{
public:
	virtual ~IOpenGLRenderer() = default;

	virtual void EnableMultisampling(bool enable) = 0;
};