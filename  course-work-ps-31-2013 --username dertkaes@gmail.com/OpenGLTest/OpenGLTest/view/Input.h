#pragma once

class CInput
{
public:
	CInput(void);
	~CInput(void);
	
	static void OnSpecialKeyPress(int key, int x, int y);
	static void OnMouse(int button, int state, int x, int y);
	static void OnKeyboard(unsigned char key, int x, int y);
	static void OnPassiveMouseMove(int x, int y);
};

