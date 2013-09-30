#pragma once

class IObject
{
public:
	virtual void Draw() = 0;
	virtual ~IObject() {}
};