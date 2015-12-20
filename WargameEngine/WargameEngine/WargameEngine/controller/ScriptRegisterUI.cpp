#include "ScriptRegisterFunctions.h"
#include "IScriptHandler.h"
#include "ScriptUIProtocol.h"
#include "../UI/IUI.h"
#include "../view/GameView.h"

void RegisterUI(IScriptHandler & handler, CGameView & view)
{
	handler.RegisterMethod(CLASS_UI, NEW_BUTTON, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 7)
			throw std::runtime_error("7 arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		std::string name = args.GetStr(1);
		int x = args.GetInt(2);
		int y = args.GetInt(3);
		int height = args.GetInt(4);
		int width = args.GetInt(5);
		std::string text = args.GetStr(6);
		std::string callback = args.GetStr(7);
		auto onClick = [callback, &handler]()
		{ 
			handler.CallFunction(callback);
		};
		auto& transMan = view.GetTranslationManager();
		IUIElement * button = c->AddNewButton(name, x, y, height, width, transMan.GetTranslation(text), onClick);
		return FunctionArgument(button, "UI");
	});

	handler.RegisterMethod(CLASS_UI, NEW_STATIC_TEXT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 6)
			throw std::runtime_error("6 arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		std::string name = args.GetStr(1);
		int x = args.GetInt(2);
		int y = args.GetInt(3);
		int height = args.GetInt(4);
		int width = args.GetInt(5);
		std::string caption = args.GetStr(6);
		auto& transMan = view.GetTranslationManager();
		IUIElement * text = c->AddNewStaticText(name, x, y, height, width, transMan.GetTranslation(caption));
		return FunctionArgument(text, "UI");
	});

	handler.RegisterMethod(CLASS_UI, NEW_PANEL, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		std::string name = args.GetStr(1);
		int x = args.GetInt(2);
		int y = args.GetInt(3);
		int height = args.GetInt(4);
		int width = args.GetInt(5);
		IUIElement * panel = c->AddNewPanel(name, x, y, height, width);
		return FunctionArgument(panel, "UI");
	});

	handler.RegisterMethod(CLASS_UI, NEW_CHECKBOX, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 7)
			throw std::runtime_error("7 arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		std::string name = args.GetStr(1);
		int x = args.GetInt(2);
		int y = args.GetInt(3);
		int height = args.GetInt(4);
		int width = args.GetInt(5);
		std::string caption = args.GetStr(6);
		bool state = args.GetBool(7);
		auto& transMan = view.GetTranslationManager();
		IUIElement * checkbox = c->AddNewCheckBox(name, x, y, height, width, transMan.GetTranslation(caption), state);
		return FunctionArgument(checkbox, "UI");
	});

	handler.RegisterMethod(CLASS_UI, NEW_COMBOBOX, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		std::string name = args.GetStr(1);
		int x = args.GetInt(2);
		int y = args.GetInt(3);
		int height = args.GetInt(4);
		int width = args.GetInt(5);
		IUIElement * listbox = c->AddNewComboBox(name, x, y, height, width);
		return FunctionArgument(listbox, "UI");
	});

	handler.RegisterMethod(CLASS_UI, NEW_EDIT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 6)
			throw std::runtime_error("6 arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		std::string name = args.GetStr(1);
		int x = args.GetInt(2);
		int y = args.GetInt(3);
		int height = args.GetInt(4);
		int width = args.GetInt(5);
		std::string caption = args.GetStr(6);
		auto& transMan = view.GetTranslationManager();
		IUIElement * edit = c->AddNewEdit(name, x, y, height, width, transMan.GetTranslation(caption));
		return FunctionArgument(edit, "UI");
	});

	handler.RegisterMethod(CLASS_UI, NEW_LIST, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		std::string name = args.GetStr(1);
		int x = args.GetInt(2);
		int y = args.GetInt(3);
		int height = args.GetInt(4);
		int width = args.GetInt(5);
		IUIElement * listbox = c->AddNewList(name, x, y, height, width);
		return FunctionArgument(listbox, "UI");
	});

	handler.RegisterMethod(CLASS_UI, NEW_RADIOGROUP, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		std::string name = args.GetStr(1);
		int x = args.GetInt(2);
		int y = args.GetInt(3);
		int height = args.GetInt(4);
		int width = args.GetInt(5);
		IUIElement * listbox = c->AddNewRadioGroup(name, x, y, height, width);
		return FunctionArgument(listbox, "UI");
	});

	handler.RegisterMethod(CLASS_UI, NEW_WINDOW, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 5)
			throw std::runtime_error("5 arguments expected (name, width, height, headerText, modal");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		std::string name = args.GetStr(1);
		int height = args.GetInt(2);
		int width = args.GetInt(3);
		std::string header = args.GetStr(4);
		bool modal = args.GetBool(5); modal;
		auto& transMan = view.GetTranslationManager();
		IUIElement * panel = c->AddNewWindow(name, height, width, transMan.GetTranslation(header));
		return FunctionArgument(panel, "UI");
	});

	handler.RegisterMethod(CLASS_UI, GET_CHILD, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (childname)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		std::string name = args.GetStr(1);
		return FunctionArgument(c->GetChildByName(name), "UI");
	});

	handler.RegisterMethod(CLASS_UI, SET_VISIBLE, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (visibility)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		c->SetVisible(args.GetBool(1));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, GET_VISIBLE, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		return c->GetVisible();
	});

	handler.RegisterMethod(CLASS_UI, SET_TEXT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (text)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		auto& transMan = view.GetTranslationManager();
		c->SetText(transMan.GetTranslation(args.GetStr(1)));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, GET_TEXT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		return c->GetText();
	});

	handler.RegisterMethod(CLASS_UI, GET_STATE, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		return c->GetState();
	});

	handler.RegisterMethod(CLASS_UI, ADD_ITEM, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (item)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		auto& transMan = view.GetTranslationManager();
		c->AddItem(transMan.GetTranslation(args.GetStr(1)));
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, DELETE_ITEM, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (index)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		size_t index = static_cast<unsigned int>(args.GetLong(1));
		if(index >= 0 && index < c->GetItemsCount())
		{
			c->DeleteItem(index);		
		}
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, GET_SELECTED_INDEX, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		return c->GetSelectedIndex() + 1;
	});

	handler.RegisterMethod(CLASS_UI, GET_ITEMS_COUNT, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		return (long)c->GetItemsCount();
	});

	handler.RegisterMethod(CLASS_UI, GET_ITEM, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (index)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		int index = args.GetInt(1) - 1;
		return c->GetItem(index);
	});

	handler.RegisterMethod(CLASS_UI, CLEAR_ITEMS, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		c->ClearItems();
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, SET_SELECTED_INDEX, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (index)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		int index = args.GetInt(1) - 1;
		c->SetSelected(index);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, SET_ON_CHANGE_CALLBACK, [&](void* instance, IArguments const& args) {
		if(args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (funcName)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		std::string func = args.GetStr(1);
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

	handler.RegisterMethod(CLASS_UI, SET_ON_CLICK_CALLBACK, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (funcName)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		std::string func = args.GetStr(1);
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

	handler.RegisterMethod(CLASS_UI, SET_BACKGROUND_IMAGE, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (image)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		std::string image = args.GetStr(1);
		c->SetBackgroundImage(image);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, GET, [&](void* /*instance*/, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		return FunctionArgument(view.GetUI(), "UI");
	});

	handler.RegisterMethod(CLASS_UI, CLEAR_CHILDREN, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 0)
			throw std::runtime_error("no arguments expected");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		c->ClearChildren();
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, DELETE_CHILD, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (name)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		std::string name = args.GetStr(1);
		c->DeleteChild(name);
		return nullptr;
	});

	handler.RegisterMethod(CLASS_UI, APPLY_THEME, [&](void* instance, IArguments const& args) {
		if (args.GetCount() != 1)
			throw std::runtime_error("1 argument expected (theme file)");
		IUIElement * c = instance ? reinterpret_cast<IUIElement*>(instance) : view.GetUI();
		std::string file = args.GetStr(1);
		std::shared_ptr<CUITheme> theme = std::make_shared<CUITheme>(CUITheme());
		theme->Load(file);
		c->SetTheme(theme);
		return nullptr;
	});
}