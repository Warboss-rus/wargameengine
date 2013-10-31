#include "LUAScriptHandler.h"
#include "..\UI\UIElement.h"
#include "..\UI\UICheckBox.h"
#include "..\UI\UIListBox.h"
#include "..\view\GameView.h"

int NewUI(lua_State* L)
{
	return CLUAScript::NewInstanceClass(new CUIElement(), "UI");
}

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

int NewListbox(lua_State* L)
{
	if (CLUAScript::GetArgumentCount() != 6)
        return luaL_error(L, "5 arguments expected");
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
	std::string name = CLUAScript::GetArgument<char*>(2);
	int x = CLUAScript::GetArgument<int>(3);
	int y = CLUAScript::GetArgument<int>(4);
	int height = CLUAScript::GetArgument<int>(5);
	int width = CLUAScript::GetArgument<int>(6);
	IUIElement * listbox = c->AddNewListBox(name, x, y, height, width);
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
	CUIListBox * c = (CUIListBox *)CLUAScript::GetClassInstance("UI");
	c->AddItem(CLUAScript::GetArgument<char*>(2));
	return 0;
}

int Free(lua_State* L)
{
	IUIElement * c = (IUIElement *)CLUAScript::GetClassInstance("UI");
    delete c;
	c = nullptr;
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

static const luaL_Reg UIFuncs[] = {
    { "New", NewUI },
	{ "NewButton", NewButton },
	{ "NewStaticText", NewStaticText },
	{ "NewPanel", NewPanel },
	{ "NewCheckbox", NewCheckbox },
	{ "NewListbox", NewListbox },
	{ "NewEdit", NewEdit },
	{ "GetChild", GetChild },
	{ "SetVisible", SetVisible },
	{ "GetVisible", GetVisible },
	{ "GetText", GetText },
	{ "GetState", GetState },
	{ "AddItem", AddItem },
	{ "Set", Set },
	{ "Get", Get },
	{ "Free", Free },
    { NULL, NULL }
};

void RegisterUI(CLUAScript & lua)
{
	lua.RegisterClass(UIFuncs, "UI");
}