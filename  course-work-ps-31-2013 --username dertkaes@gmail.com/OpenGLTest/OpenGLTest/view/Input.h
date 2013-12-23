#pragma once


class CInput
{
private:
	static const int BACKSPACE_BUTTON_ID = 8;
	static const int SCROLL_UP = 3;
	static const int SCROLL_DOWN = 4;

	static bool m_isLMBDown;
	static bool m_isRMBDown;
	static bool m_ruler;
	static double startX;
	static double startY;
	static double m_oldRotation;
public:

	static void EnableRuler() { m_ruler = true; }
	
	static void OnSpecialKeyPress(int key, int x, int y);
	static void OnMouse(int button, int state, int x, int y);
	static void OnKeyboard(unsigned char key, int x, int y);
	static void OnPassiveMouseMove(int x, int y);
	static void OnMouseMove(int x, int y);
};

