#include "LUAScriptHandler.h"
#include "../UI/UIElement.h"
#include "../UI/UICheckBox.h"
#include "../UI/UIComboBox.h"
#include "../view/GameView.h"

IUIElement * GetUIPointer()
{
	if (CLUAScript::IsClassInstance())//it is UI instance, not metatable
		return (IUIElement *)CLUAScript::GetClassInstance("UI");
	else
		return CGameView::GetInstance().lock()->GetUI();
}

int NewButton(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 8)
        return luaL_error(L, "7 arguments expected");
	IUIElement * c = GetUIPointer();
	std::string name = CLUAScript::GetArgument<char*>(2);
	int x = CLUAScript::GetArgument<int>(3);
	int y = CLUAScript::GetArgument<int>(4);
	int height = CLUAScript::GetArgument<int>(5);
	int width = CLUAScript::GetArgument<int>(6);
	char* text = CLUAScript::GetArgument<char*>(7);
	std::string callback = CLUAScript::GetArgument<char*>(8);
	auto onClick = [callback]()
	{ 
		CLUAScript::CallFunction(callback);
	};
	IUIElement * button = c->AddNewButton(name, x, y, height, width, text, onClick);
	return CLUAScript::NewInstanceClass(button, "UI");
}

int NewStaticText(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 7)
        return luaL_error(L, "6 arguments expected");
	IUIElement * c = GetUIPointer();
	std::string name = CLUAScript::GetArgument<char*>(2);
	int x = CLUAScript::GetArgument<int>(3);
	int y = CLUAScript::GetArgument<int>(4);
	int height = CLUAScript::GetArgument<int>(5);
	int width = CLUAScript::GetArgument<int>(6);
	char* caption = CLUAScript::GetArgument<char*>(7);
	IUIElement * text = c->AddNewStaticText(name, x, y, height, width, caption);
	return CLUAScript::NewInstanceClass(text, "UI");
}

int NewPanel(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 6)
        return luaL_error(L, "5 arguments expected");
	IUIElement * c = GetUIPointer();
	std::string name = CLUAScript::GetArgument<char*>(2);
	int x = CLUAScript::GetArgument<int>(3);
	int y = CLUAScript::GetArgument<int>(4);
	int height = CLUAScript::GetArgument<int>(5);
	int width = CLUAScript::GetArgument<int>(6);
	IUIElement * panel = c->AddNewPanel(name, x, y, height, width);
	return CLUAScript::NewInstanceClass(panel, "UI");
}

int NewCheckbox(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 8)
        return luaL_error(L, "7 arguments expected");
	IUIElement * c = GetUIPointer();
	std::string name = CLUAScript::GetArgument<char*>(2);
	int x = CLUAScript::GetArgument<int>(3);
	int y = CLUAScript::GetArgument<int>(4);
	int height = CLUAScript::GetArgument<int>(5);
	int width = CLUAScript::GetArgument<int>(6);
	char* caption = CLUAScript::GetArgument<char*>(7);
	bool state = CLUAScript::GetArgument<bool>(8);
	IUIElement * checkbox = c->AddNewCheckBox(name, x, y, height, width, caption, state);
	return CLUAScript::NewInstanceClass(checkbox, "UI");
}

int NewCombobox(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 6)
        return luaL_error(L, "5 arguments expected");
	IUIElement * c = GetUIPointer();
	std::string name = CLUAScript::GetArgument<char*>(2);
	int x = CLUAScript::GetArgument<int>(3);
	int y = CLUAScript::GetArgument<int>(4);
	int height = CLUAScript::GetArgument<int>(5);
	int width = CLUAScript::GetArgument<int>(6);
	IUIElement * listbox = c->AddNewComboBox(name, x, y, height, width);
	return CLUAScript::NewInstanceClass(listbox, "UI");
}

int NewEdit(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 7)
        return luaL_error(L, "6 arguments expected");
	IUIElement * c = GetUIPointer();
	std::string name = CLUAScript::GetArgument<char*>(2);
	int x = CLUAScript::GetArgument<int>(3);
	int y = CLUAScript::GetArgument<int>(4);
	int height = CLUAScript::GetArgument<int>(5);
	int width = CLUAScript::GetArgument<int>(6);
	char* caption = CLUAScript::GetArgument<char*>(7);
	IUIElement * edit = c->AddNewEdit(name, x, y, height, width, caption);
	return CLUAScript::NewInstanceClass(edit, "UI");
}

int NewList(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 6)
        return luaL_error(L, "5 arguments expected");
	IUIElement * c = GetUIPointer();
	std::string name = CLUAScript::GetArgument<char*>(2);
	int x = CLUAScript::GetArgument<int>(3);
	int y = CLUAScript::GetArgument<int>(4);
	int height = CLUAScript::GetArgument<int>(5);
	int width = CLUAScript::GetArgument<int>(6);
	IUIElement * listbox = c->AddNewList(name, x, y, height, width);
	return CLUAScript::NewInstanceClass(listbox, "UI");
}

int NewRadioGroup(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 6)
        return luaL_error(L, "5 arguments expected");
	IUIElement * c = GetUIPointer();
	std::string name = CLUAScript::GetArgument<char*>(2);
	int x = CLUAScript::GetArgument<int>(3);
	int y = CLUAScript::GetArgument<int>(4);
	int height = CLUAScript::GetArgument<int>(5);
	int width = CLUAScript::GetArgument<int>(6);
	IUIElement * listbox = c->AddNewRadioGroup(name, x, y, height, width);
	return CLUAScript::NewInstanceClass(listbox, "UI");
}

int GetChild(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected (childname)");
	IUIElement * c = GetUIPointer();
	std::string name = CLUAScript::GetArgument<char*>(2);
	return CLUAScript::NewInstanceClass(c->GetChildByName(name), "UI");
}

int SetVisible(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected (visibility)");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	c->SetVisible(CLUAScript::GetArgument<bool>(2));
	return 0;
}

int SetText(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected (text)");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	try
	{
		c->SetText(CLUAScript::GetArgument<char*>(2));
	}
	catch (std::runtime_error const& e)
	{
		return luaL_error(L, e.what());
	}
	return 0;
}

int GetVisible(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no arguments expected");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	CLUAScript::SetArgument(c->GetVisible());
	return 1;
}

int GetText(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no arguments expected");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	CLUAScript::SetArgument(c->GetText().c_str());
	return 1;
}

int GetState(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no arguments expected");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	CLUAScript::SetArgument(c->GetState());
	return 1;
}

int AddItem(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected (item)");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	try
	{
		c->AddItem(CLUAScript::GetArgument<char*>(2));
	}
	catch (std::runtime_error const& e)
	{
		return luaL_error(L, e.what());
	}
	return 0;
}

int DeleteItem(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected (index)");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	unsigned int index = CLUAScript::GetArgument<unsigned int>(2);
	if(index >= 0 && index < c->GetItemsCount())
	{
		try
		{
			c->DeleteItem(index);
		}
		catch (std::runtime_error const& e)
		{
			return luaL_error(L, e.what());
		}
		
	}
	return 0;
}

int GetSelectedIndex(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no arguments expected");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	try
	{
		CLUAScript::SetArgument(c->GetSelectedIndex() + 1);
	}
	catch (std::runtime_error const& e)
	{
		return luaL_error(L, e.what());
	}
	return 1;
}

int GetItemsCount(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no arguments expected");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	try
	{
		CLUAScript::SetArgument((int)c->GetItemsCount());
	}
	catch (std::runtime_error const& e)
	{
		return luaL_error(L, e.what());
	}
	return 1;
}

int GetItem(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected (index)");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	int index = CLUAScript::GetArgument<int>(2) - 1;
	try
	{
		CLUAScript::SetArgument(c->GetItem(index).c_str());
	}
	catch (std::runtime_error const& e)
	{
		return luaL_error(L, e.what());
	}
	return 1;
}

int ClearItems(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no arguments expected");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	try
	{
		c->ClearItems();
	}
	catch (std::runtime_error const& e)
	{
		return luaL_error(L, e.what());
	}
	return 0;
}

int SetSelectedIndex(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected (index)");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	int index = CLUAScript::GetArgument<int>(2) - 1;
	c->SetSelected(index);
	return 0;
}

int SetOnChangeCallback(lua_State* L)
{
	if(CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "1 argument expected (funcName)");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	std::string func = CLUAScript::GetArgument<char*>(2);
	std::function<void()> function;
	if(!func.empty())
	{
		function = [func]()
		{ 
			CLUAScript::CallFunction(func);
		};
	}
	try
	{
		c->SetOnChangeCallback(function);
	}
	catch (std::exception const& e)
	{
		return luaL_error(L, e.what());
	}
	return 0;
}

int SetOnClickCallback(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "1 argument expected (funcName)");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	std::string func = CLUAScript::GetArgument<char*>(2);
	std::function<void()> function;
	if (!func.empty())
	{
		function = [func]()
		{
			CLUAScript::CallFunction(func);
		};
	}
	try
	{
		c->SetOnClickCallback(function);
	}
	catch (std::exception const& e)
	{
		return luaL_error(L, e.what());
	}
	return 0;
}

int SetBackgroundImage(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "1 argument expected (image)");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	std::string image = CLUAScript::GetArgument<char*>(2);
	try 
	{
		c->SetBackgroundImage(image);
	}
	catch (std::exception const& e)
	{
		return luaL_error(L, e.what());
	}
	return 0;
}

int Get(lua_State* L)
{
	return CLUAScript::NewInstanceClass(CGameView::GetInstance().lock()->GetUI(), "UI");
}

int ClearChildren(lua_State* L)
{
	IUIElement * c = GetUIPointer();
	c->ClearChildren();
	return 0;
}

int DeleteChild(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected (name)");
	IUIElement * c = GetUIPointer();
	std::string name = CLUAScript::GetArgument<char*>(2);
	c->DeleteChild(name);
	return 0;
}

int ApplyTheme(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
		return luaL_error(L, "1 argument expected (theme file)");
	IUIElement * c = GetUIPointer();
	std::string file = CLUAScript::GetArgument<char*>(2);
	std::shared_ptr<CUITheme> theme = std::make_shared<CUITheme>(CUITheme());
	theme->Load(file);
	c->SetTheme(theme);
	return 0;
}

int Getter(lua_State* L)
{
	std::string key = CLUAScript::GetKeyForGetter();
	if (key.empty())
	{
		return 0;
	}
	//return luaL_error(L, ("key " + key + "does not exist").c_str());
	if (key == "text")
		return GetText(L);
	if (key == "visible")
		return GetVisible(L);
	if (key == "state")
		return GetState(L);
	if (key == "selectedIndex")
		return GetSelectedIndex(L);
	if (key == "itemsCount")
		return GetItemsCount(L);
	return GetChild(L);
}

int Setter(lua_State* L)
{
	std::string key = CLUAScript::GetArgument<const char*>(2);
	if (key == "__self")
	{
		return 1;
	}
	std::string value = CLUAScript::GetArgument<const char*>(3);
	return 0;
}

static const luaL_Reg UIFuncs[] = {
   	{ "NewButton", NewButton },
	{ "NewStaticText", NewStaticText },
	{ "NewPanel", NewPanel },
	{ "NewCheckbox", NewCheckbox },
	{ "NewCombobox", NewCombobox },
	{ "NewEdit", NewEdit },
	{ "NewList", NewList },
	{ "NewRadioGroup", NewRadioGroup },
	{ "GetChild", GetChild },
	{ "SetVisible", SetVisible },
	{ "GetVisible", GetVisible },
	{ "GetText", GetText },
	{ "SetText", SetText },
	{ "GetState", GetState },
	{ "AddItem", AddItem },
	{ "DeleteItem", DeleteItem },
	{ "GetSelectedIndex", GetSelectedIndex },
	{ "SetSelectedIndex", SetSelectedIndex },
	{ "GetItemsCount", GetItemsCount },
	{ "GetItem", GetItem },
	{ "ClearItems", ClearItems },
	{ "Get", Get },
	{ "ClearChildren", ClearChildren },
	{ "DeleteChild", DeleteChild },
	{ "SetOnChangeCallback", SetOnChangeCallback },
	{ "SetOnClickCallback", SetOnClickCallback },
	{ "SetBackgroundImage", SetBackgroundImage },
	{ "ApplyTheme", ApplyTheme },
	//{ "__index",  Getter},
	//{ "__newindex", Setter },
	{ NULL, NULL }
};

void RegisterUI(CLUAScript & lua)
{
	lua.RegisterClass(UIFuncs, "UI");
}