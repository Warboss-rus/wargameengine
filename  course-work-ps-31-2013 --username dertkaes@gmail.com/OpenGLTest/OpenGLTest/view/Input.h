#pragma once

class CInput
{
private:
	static bool m_isLMBDown;
	static bool m_ruler;
public:

	static void EnableRuler() { m_ruler = true; }
	
	static void OnSpecialKeyPress(int key, int x, int y);
	static void OnMouse(int button, int state, int x, int y);
	static void OnKeyboard(unsigned char key, int x, int y);
	static void OnPassiveMouseMove(int x, int y);
	static void OnMouseMove(int x, int y);
};

