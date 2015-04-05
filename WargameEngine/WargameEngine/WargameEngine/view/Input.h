#include <map>
#include <memory>
#include <functional>
#pragma once
#include <string>

class IObject;

class CInput
{
private:
	
	static const int SCROLL_UP = 3;
	static const int SCROLL_DOWN = 4;

	static bool m_isLMBDown;
	static bool m_isRMBDown;
	static bool m_ruler;
	static double startX;
	static double startY;
	static int startWindowX;
	static int startWindowY;
	static double m_oldRotation;
	typedef std::function<void(std::shared_ptr<IObject>, std::string const&, double, double, double)> MouseCallback;
	static MouseCallback m_LMBclickCallback;
	static MouseCallback m_RMBclickCallback;
	static bool m_disableDefaultLMB;
	static bool m_disableDefaultRMB;
public:

	static void EnableRuler() { m_ruler = true; }
	
	static void OnSpecialKeyPress(int key, int x, int y);
	static void OnMouse(int button, int state, int x, int y);
	static void OnKeyboard(unsigned char key, int x, int y);
	static void OnPassiveMouseMove(int x, int y);
	static void OnMouseMove(int x, int y);
	static void SetLMBCallback(MouseCallback const& callback, bool disableDefault);
	static void SetRMBCallback(MouseCallback const& callback, bool disableDefault);
};
