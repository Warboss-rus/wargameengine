#include <memory>
#include <string>
#include <vector>
#include "UITheme.h"
#define CPP11CALLBACK
#ifdef CPP11CALLBACK
#include <functional>
#define callback(x) std::function<void()>(x)
#else
#define callback(x) void(*x)()
#endif

class IUIElement
{
public:
	virtual void Draw() const = 0;
	virtual void AddChild(std::string const& name, std::shared_ptr<IUIElement> element) = 0;
	virtual IUIElement* GetChildByName(std::string const& name) = 0;
	virtual void DeleteChild(std::string const& name) = 0;
	virtual bool LeftMouseButtonDown(int x, int y) = 0;
	virtual bool LeftMouseButtonUp(int x, int y) = 0;
	virtual bool OnKeyPress(unsigned char key) = 0;
	virtual bool OnSpecialKeyPress(int key) = 0;
	virtual int GetX() const = 0;
	virtual int GetY() const = 0;
	virtual int GetHeight() const = 0;
	virtual int GetWidth() const = 0;
	virtual void SetVisible(bool visible) = 0;
	virtual bool GetVisible() = 0;
	virtual bool IsFocused(const IUIElement * child) const = 0;
	virtual void SetTheme(CUITheme const& theme) = 0;
	virtual CUITheme & GetTheme() = 0;
	virtual std::string const& GetText() const = 0;
	virtual void SetText(std::string const& text) = 0;

	virtual IUIElement* AddNewButton(std::string const& name, int x, int y, int height, int width, char* text, callback(onClick)) = 0;
	virtual IUIElement* AddNewStaticText(std::string const& name, int x, int y, int height, int width, char* text) = 0;
	virtual IUIElement* AddNewPanel(std::string const& name, int x, int y, int height, int width) = 0;
	virtual IUIElement* AddNewCheckBox(std::string const& name, int x, int y, int height, int width, char* text, bool initState) = 0;
	virtual IUIElement* AddNewListBox(std::string const& name, int x, int y, int height, int width) = 0;
	virtual IUIElement* AddNewListBox(std::string const& name, int x, int y, int height, int width, std::vector<std::string> items) = 0;
	virtual IUIElement* AddNewEdit(std::string const& name, int x, int y, int height, int width, char* text) = 0;
	virtual void SetFocus(IUIElement * focus = NULL) = 0;
};