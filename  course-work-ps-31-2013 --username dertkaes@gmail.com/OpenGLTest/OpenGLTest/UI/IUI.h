#include <memory>
#include <string>

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
};