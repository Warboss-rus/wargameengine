#include "ScriptRegisterFunctions.h"
#include "IScriptHandler.h"
#include "ScriptUIProtocol.h"
#include "../UI/IUI.h"
#include "../view/TranslationManager.h"
#include "../UI/UITheme.h"
#include "../Utils.h"

void RegisterUI(IScriptHandler & handler, IUIElement * uiRoot, CTranslationManager & transMan)
{
	handler.RegisterMethod(CLASS_UI, NEW_BUTTON, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 7)
			throw std::runtime_error("7 arguments expected(name, x, y, height, width, text, callback)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		std::string name = args.GetStr(1);
		int x = args.GetInt(2);
		int y = args.GetInt(3);
		int height = args.GetInt(4);
		int width = args.GetInt(5);
		std::wstring text = args.GetWStr(6);
		std::wstring callback = args.GetWStr(7);
		auto onClick = [callback, &handler]()
		{ 
			handler.CallFunction(callback);
		};
		IUIElement * button = c->AddNewButton(name, x, y, height, width, transMan.GetTranslation(text), onClick);
		return FunctionArgument(button, L"UI");
	});

	handler.RegisterMethod(CLASS_UI, NEW_STATIC_TEXT, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 6)
			throw std::runtime_error("6 arguments expected (name, x, y, height, width, text");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		std::string name = args.GetStr(1);
		int x = args.GetInt(2);
		int y = args.GetInt(3);
		int height = args.GetInt(4);
		int width = args.GetInt(5);
		std::wstring caption = args.GetWStr(6);
		IUIElement * text = c->AddNewStaticText(name, x, y, height, width, transMan.GetTranslation(caption));
		return FunctionArgument(text, L"UI");
	});

	handler.RegisterMethod(CLASS_UI, NEW_PANEL, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 arguments expected (name, x, y, height, width)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		std::string name = args.GetStr(1);
		int x = args.GetInt(2);
		int y = args.GetInt(3);
		int height = args.GetInt(4);
		int width = args.GetInt(5);
		IUIElement * panel = c->AddNewPanel(name, x, y, height, width);
		return FunctionArgument(panel, L"UI");
	});

	handler.RegisterMethod(CLASS_UI, NEW_CHECKBOX, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 7)
			throw std::runtime_error("7 arguments expected(name, x, y, height, width, text, initState)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		std::string name = args.GetStr(1);
		int x = args.GetInt(2);
		int y = args.GetInt(3);
		int height = args.GetInt(4);
		int width = args.GetInt(5);
		std::wstring caption = args.GetWStr(6);
		bool state = args.GetBool(7);
		IUIElement * checkbox = c->AddNewCheckBox(name, x, y, height, width, transMan.GetTranslation(caption), state);
		return FunctionArgument(checkbox, L"UI");
	});

	handler.RegisterMethod(CLASS_UI, NEW_COMBOBOX, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 arguments expected (name, x, y, height, width)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		std::string name = args.GetStr(1);
		int x = args.GetInt(2);
		int y = args.GetInt(3);
		int height = args.GetInt(4);
		int width = args.GetInt(5);
		IUIElement * listbox = c->AddNewComboBox(name, x, y, height, width);
		return FunctionArgument(listbox, L"UI");
	});

	handler.RegisterMethod(CLASS_UI, NEW_EDIT, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 6)
			throw std::runtime_error("6 arguments expected (name, x, y, height, width, text)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		std::string name = args.GetStr(1);
		int x = args.GetInt(2);
		int y = args.GetInt(3);
		int height = args.GetInt(4);
		int width = args.GetInt(5);
		std::wstring caption = args.GetWStr(6);
		IUIElement * edit = c->AddNewEdit(name, x, y, height, width, transMan.GetTranslation(caption));
		return FunctionArgument(edit, L"UI");
	});

	handler.RegisterMethod(CLASS_UI, NEW_LIST, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 arguments expected (name, x, y, height, width)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		std::string name = args.GetStr(1);
		int x = args.GetInt(2);
		int y = args.GetInt(3);
		int height = args.GetInt(4);
		int width = args.GetInt(5);
		IUIElement * listbox = c->AddNewList(name, x, y, height, width);
		return FunctionArgument(listbox, L"UI");
	});

	handler.RegisterMethod(CLASS_UI, NEW_RADIOGROUP, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 arguments expected (name, x, y, height, width)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		std::string name = args.GetStr(1);
		int x = args.GetInt(2);
		int y = args.GetInt(3);
		int height = args.GetInt(4);
		int width = args.GetInt(5);
		IUIElement * listbox = c->AddNewRadioGroup(name, x, y, height, width);
		return FunctionArgument(listbox, L"UI");
	});

	handler.RegisterMethod(CLASS_UI, NEW_WINDOW, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 arguments expected (name, width, height, headerText, modal");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		std::string name = args.GetStr(1);
		int height = args.GetInt(2);
		int width = args.GetInt(3);
		std::wstring header = args.GetWStr(4);
		bool modal = args.GetBool(5); modal;
		IUIElement * panel = c->AddNewWindow(name, height, width, transMan.GetTranslation(header));
		return FunctionArgument(panel, L"UI");
	});

	handler.RegisterMethod(CLASS_UI, GET_CHILD, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (childname)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		std::string name = args.GetStr(1);
		return FunctionArgument(c->GetChildByName(name), L"UI");
	});

	handler.RegisterMethod(CLASS_UI, SET_VISIBLE, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (visibility)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		c->SetVisible(args.GetBool(1));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, GET_VISIBLE, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		return c->GetVisible();
	});

	handler.RegisterMethod(CLASS_UI, SET_TEXT, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (text)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		c->SetText(transMan.GetTranslation(args.GetWStr(1)));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, GET_TEXT, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		return c->GetText();
	});

	handler.RegisterMethod(CLASS_UI, GET_STATE, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		return c->GetState();
	});

	handler.RegisterMethod(CLASS_UI, ADD_ITEM, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (item)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		c->AddItem(transMan.GetTranslation(args.GetWStr(1)));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, DELETE_ITEM, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (index)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		size_t index = args.GetSizeT(1);
		if(index < c->GetItemsCount())
		{
			c->DeleteItem(index);		
		}
		else
		{
			throw std::runtime_error("Invalid index:" + std::to_string(index) + ". Element only have " + std::to_string(c->GetItemsCount()) + " items.");
		}
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, GET_SELECTED_INDEX, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		return static_cast<int>(c->GetSelectedIndex() + 1);
	});

	handler.RegisterMethod(CLASS_UI, GET_ITEMS_COUNT, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		return (int)c->GetItemsCount();
	});

	handler.RegisterMethod(CLASS_UI, GET_ITEM, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (index)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		size_t index = args.GetSizeT(1) - 1;
		if (index < c->GetItemsCount())
		{
			return c->GetItem(index);
		}
		throw std::runtime_error("Invalid index:" + std::to_string(index) + ". Element only have " + std::to_string(c->GetItemsCount()) + " items.");
	});

	handler.RegisterMethod(CLASS_UI, CLEAR_ITEMS, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		c->ClearItems();
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, SET_SELECTED_INDEX, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (index)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		size_t index = args.GetSizeT(1) - 1;
		if (index < c->GetItemsCount())
		{
			c->SetSelected(index);
		}
		else
		{
			throw std::runtime_error("Invalid index");
		}
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, SET_ON_CHANGE_CALLBACK, [&, uiRoot](void* instance, IArguments const& args) {
		if(args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (funcName)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		std::wstring func = args.GetWStr(1);
		std::function<void()> function;
		if(!func.empty())
		{
			function = [func, &handler]()
			{ 
				handler.CallFunction(func);
			};
		}
		c->SetOnChangeCallback(function);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, SET_ON_CLICK_CALLBACK, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (funcName)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		std::wstring func = args.GetWStr(1);
		std::function<void()> function;
		if (!func.empty())
		{
			function = [func, &handler]()
			{
				handler.CallFunction(func);
			};
		}
		c->SetOnClickCallback(function);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, SET_BACKGROUND_IMAGE, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (image)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		std::string image = args.GetStr(1);
		c->SetBackgroundImage(image);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, GET, [&, uiRoot](void* /*instance*/, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		return FunctionArgument(uiRoot, L"UI");
	});

	handler.RegisterMethod(CLASS_UI, CLEAR_CHILDREN, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		c->ClearChildren();
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, DELETE_CHILD, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (name)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		std::string name = args.GetStr(1);
		c->DeleteChild(name);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, APPLY_THEME, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (theme file)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		std::wstring file = args.GetWStr(1);
		std::shared_ptr<CUITheme> theme = std::make_shared<CUITheme>();
		theme->Load(file);
		c->SetTheme(theme);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, SET_TARGET_SIZE, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 2)
			throw std::runtime_error("2 arguments expected (width, height)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		int width = args.GetInt(1);
		int height = args.GetInt(2);
		c->SetTargetSize(width, height);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, SET_SCALE, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (scale)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		float scale = args.GetFloat(1);
		c->SetScale(scale);
		return nullptr;
	});

	handler.RegisterProperty(CLASS_UI, PROPERTY_VISIBLE, [&, uiRoot](void* instance, IArguments const& args){
		if (args.GetCount() != 1) throw std::runtime_error("1 value expected (visible)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		c->SetVisible(args.GetBool(1));
	}, [&, uiRoot](void* instance) {
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		return c->GetVisible();
	});

	handler.RegisterProperty(CLASS_UI, PROPERTY_TEXT, [&, uiRoot](void* instance, IArguments const& args) {
		if (args.GetCount() != 1) throw std::runtime_error("1 value expected (text)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		c->SetText(transMan.GetTranslation(args.GetWStr(1)));
	}, [&, uiRoot](void* instance) {
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		return c->GetText();
	});

	handler.RegisterProperty(CLASS_UI, PROPERTY_X, [&, uiRoot](void* instance) {
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		return c->GetX();
	});

	handler.RegisterProperty(CLASS_UI, PROPERTY_Y, [&, uiRoot](void* instance) {
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		return c->GetY();
	});

	handler.RegisterProperty(CLASS_UI, PROPERTY_WIDTH, [&, uiRoot](void* instance) {
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		return c->GetWidth();
	});

	handler.RegisterProperty(CLASS_UI, PROPERTY_HEIGHT, [&, uiRoot](void* instance) {
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : uiRoot;
		return c->GetHeight();
	});

	handler.RegisterFunction(MESSAGE_BOX, [&, uiRoot](IArguments const& args) {
		if (args.GetCount() < 1 || args.GetCount() > 2)
			throw std::runtime_error("1 or 2 argument expected (text, caption)");
		std::wstring text = args.GetWStr(1);
		std::wstring caption;
		if (args.GetCount() == 2)
			caption = args.GetWStr(2);
		const std::string windowName = "MessageBox";
		const int width = 200;
		const int border = 10;
		const int buttonWidth = 50;
		const int buttonHeight = 20;
		auto window = uiRoot->AddNewWindow(windowName, 100, width, transMan.GetTranslation(caption));
		window->AddNewStaticText("text", border, border, window->GetHeight() - 2 * border, window->GetWidth() - 2 * border, transMan.GetTranslation(text));
		window->AddNewButton("close", (window->GetWidth() - buttonWidth) / 2, window->GetHeight() - border - buttonHeight, buttonHeight, buttonWidth, L"OK", [=] {uiRoot->DeleteChild(windowName);});
		return nullptr;
	});
}