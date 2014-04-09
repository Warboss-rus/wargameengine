#include <map>
#include <functional>
#pragma once

class CInput
{
private:
	struct sKeyBind
	{
		sKeyBind(unsigned char k, bool s, bool c, bool a):key(k), shift(s), ctrl(c), alt(a) {}
		unsigned char key;
		bool shift;
		bool ctrl;
		bool alt;
	};
	friend bool operator< (sKeyBind const& one, sKeyBind const& two);
	static const int BACKSPACE_BUTTON_ID = 8;
	static const int SCROLL_UP = 3;
	static const int SCROLL_DOWN = 4;

	static bool m_isLMBDown;
	static bool m_isRMBDown;
	static bool m_ruler;
	static double startX;
	static double startY;
	static double m_oldRotation;
	static std::map<sKeyBind, std::function<void()>> m_keyBindings;
public:

	static void EnableRuler() { m_ruler = true; }
	
	static void OnSpecialKeyPress(int key, int x, int y);
	static void OnMouse(int button, int state, int x, int y);
	static void OnKeyboard(unsigned char key, int x, int y);
	static void OnPassiveMouseMove(int x, int y);
	static void OnMouseMove(int x, int y);
	static void BindKey(unsigned char key, bool shift, bool ctrl, bool alt, std::function<void()> const& func);
};

inline bool operator< (CInput::sKeyBind const& one, CInput::sKeyBind const& two) { return one.key < two.key; }
