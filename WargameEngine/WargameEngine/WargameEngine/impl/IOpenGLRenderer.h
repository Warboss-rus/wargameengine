#pragma once
#include "..\view\IViewHelper.h"

class IOpenGLRenderer : public IViewHelper
{
public:
	virtual ~IOpenGLRenderer() = default;

	virtual void EnableMultisampling(bool enable) = 0;
};