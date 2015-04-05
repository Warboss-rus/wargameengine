#pragma once

class ICamera
{
public:
	virtual const double * GetPosition() const = 0;
	virtual const double * GetDirection() const = 0;
	virtual const double * GetUpVector() const = 0;
	virtual const double GetScale() const = 0;
	virtual bool OnKeyPress(int key) = 0;
	virtual bool OnMouseMove(int deltaX, int deltaY, bool LMB = false, bool RMB = false, bool shift = false, bool ctrl = false, bool alt = false) = 0;
	virtual bool OnMouseWheelUp() = 0;
	virtual bool OnMouseWheelDown() = 0;
	virtual bool HidePointer() const = 0;
	virtual ~ICamera() {}
};