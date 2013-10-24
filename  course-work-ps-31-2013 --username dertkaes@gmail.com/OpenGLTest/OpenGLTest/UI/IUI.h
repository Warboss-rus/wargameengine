#include <memory>
#include <string>
#include <vector>

class IUIElement
{
public:
	virtual void Draw() const = 0;
	virtual void AddChild(std::string const& name, std::shared_ptr<IUIElement> element) = 0;
	virtual std::shared_ptr<IUIElement> GetChildByName(std::string const& name) = 0;
	virtual void DeleteChild(std::string const& name) = 0;
	virtual bool LeftMouseButtonDown(int x, int y) = 0;
	virtual bool LeftMouseButtonUp(int x, int y) = 0;
	virtual int GetX() const = 0;
	virtual int GetY() const = 0;
	virtual int GetHeight() const = 0;
	virtual int GetWidth() const = 0;
	virtual void SetVisible(bool visible) = 0;
	virtual bool GetVisible() = 0;

	virtual std::shared_ptr<IUIElement> AddNewButton(std::string const& name, int x, int y, int height, int width, char* text, void (onClick)()) = 0;
	virtual std::shared_ptr<IUIElement> AddNewStaticText(std::string const& name, int x, int y, int height, int width, char* text) = 0;
	virtual std::shared_ptr<IUIElement> AddNewPanel(std::string const& name, int x, int y, int height, int width) = 0;
	virtual std::shared_ptr<IUIElement> AddNewCheckBox(std::string const& name, int x, int y, int height, int width, char* text, bool initState) = 0;
	virtual std::shared_ptr<IUIElement> AddNewListBox(std::string const& name, int x, int y, int height, int width) = 0;
	virtual std::shared_ptr<IUIElement> AddNewListBox(std::string const& name, int x, int y, int height, int width, std::vector<std::string> items) = 0;
};