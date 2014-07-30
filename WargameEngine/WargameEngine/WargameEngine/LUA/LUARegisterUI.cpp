#include "LUAScriptHandler.h"
#include "../UI/UIElement.h"
#include "../UI/UICheckBox.h"
#include "../UI/UIComboBox.h"
#include "../view/GameView.h"

int NewButton(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 8)
        return luaL_error(L, "7 arguments expected");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
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
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
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
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
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
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
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
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
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
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
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
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
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
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
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
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
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
	c->SetText(CLUAScript::GetArgument<char*>(2));
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
	CUICheckBox * c = (CUICheckBox *)CLUAScript::GetClassInstance("UI");
	CLUAScript::SetArgument(c->GetState());
	return 1;
}

int AddItem(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected (item)");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	c->AddItem(CLUAScript::GetArgument<char*>(2));
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
		c->DeleteItem(index);
	}
	return 0;
}

int GetSelectedIndex(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no arguments expected");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	CLUAScript::SetArgument(c->GetSelectedIndex() + 1);
	return 1;
}

int GetItemsCount(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no arguments expected");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	CLUAScript::SetArgument((int)c->GetItemsCount());
	return 1;
}

int GetItem(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected (index)");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	int index = CLUAScript::GetArgument<int>(2) - 1;
	CLUAScript::SetArgument(c->GetItem(index).c_str());
	return 1;
}

int ClearItems(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 1)
        return luaL_error(L, "no arguments expected");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	c->ClearItems();
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
	c->SetOnChangeCallback(function);
	return 0;
}

int Set(lua_State* L)
{
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	CGameView::GetIntanse().lock()->SetUI(c);
	return 0;
}

int Get(lua_State* L)
{
	return CLUAScript::NewInstanceClass(CGameView::GetIntanse().lock()->GetUI(), "UI");
}

int ClearChildren(lua_State* L)
{
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	c->ClearChildren();
	return 0;
}

int DeleteChild(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 2)
        return luaL_error(L, "1 argument expected (name)");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	std::string name = CLUAScript::GetArgument<char*>(2);
	c->DeleteChild(name);
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
	//{ "Set", Set },
	{ "Get", Get },
	{ "ClearChildren", ClearChildren },
	{ "DeleteChild", DeleteChild },
	{ "SetOnChangeCallback", SetOnChangeCallback },
	{ NULL, NULL }
};

void RegisterUI(CLUAScript & lua)
{
	lua.RegisterClass(UIFuncs, "UI");
}