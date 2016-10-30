#pragma once

#define CLASS_UI L"UI"

/*METHODS*/

//UI ui:NewButton(string name, int x, int y, int height, int width, string text, string callback)
//Adds a new button as a child element to the instance and returns it. If called without instance, root element will be used as the instance
#define NEW_BUTTON L"NewButton"

//UI ui:NewStaticText(string name, int x, int y, int height, int width, string text)
//Adds a new label as a child element to the instance and returns it. If called without instance, root element will be used as the instance
#define NEW_STATIC_TEXT L"NewStaticText"

//UI ui:NewPanel(string name, int x, int y, int height, int width)
//Adds a new panel as a child element to the instance and returns it. If called without instance, root element will be used as the instance
#define NEW_PANEL L"NewPanel"

//UI ui:NewCheckbox(string name, int x, int y, int height, int width, string text, bool initialState)
//Adds a new checkbox as a child element to the instance and returns it. If called without instance, root element will be used as the instance
#define NEW_CHECKBOX L"NewCheckbox"

//UI ui:NewCombobox(string name, int x, int y, int height, int width)
//Adds a new combobox as a child element to the instance and returns it. If called without instance, root element will be used as the instance
#define NEW_COMBOBOX L"NewCombobox"

//UI ui:NewEdit(string name, int x, int y, int height, int width, string defaultText)
//Adds a new edit box as a child element to the instance and returns it. If called without instance, root element will be used as the instance
#define NEW_EDIT L"NewEdit"

//UI ui:NewList(string name, int x, int y, int height, int width)
//Adds a new list box as a child element to the instance and returns it. If called without instance, root element will be used as the instance
#define NEW_LIST L"NewList"

//UI ui:NewRadioGroup(string name, int x, int y, int height, int width)
//Adds a new radiogroup as a child element to the instance and returns it. If called without instance, root element will be used as the instance
#define NEW_RADIOGROUP L"NewRadioGroup"

//UI ui:NewWindow(string name, int x, int y, int height, int width)
//Adds a new window as a child element to the instance and returns it. If called without instance, root element will be used as the instance
#define NEW_WINDOW L"NewWindow"

//UI ui:GetChild(string name)
//Returns a child UI element by a given name. Returns nil if no such child. If called without instance, root element will be used as the instance
#define GET_CHILD L"GetChild"

//void ui:SetVisible(bool visible)
//Sets whether the element is visible. Invisible elements don't displayed on the screen and don't receive input events. If called without instance, root element will be used as the instance
#define SET_VISIBLE L"SetVisible"

//bool ui:SetVisible()
//Returns whether the element is visible. Invisible elements don't displayed on the screen and don't receive input events. If called without instance, root element will be used as the instance
#define GET_VISIBLE L"GetVisible"

//string ui:GetText()
//Returns the element's text. If element has no text (for example, panel) throws an error. If element has multiple text items (example: combobox) returns the currently selected item.
//If called without instance, root element will be used as the instance
#define GET_TEXT L"GetText"

//void ui:SetText(string text)
//Sets the element's text. If element has no text (for example, panel) throws an error. If element has multiple text items (example: combobox) select the item with the same text.
//If called without instance, root element will be used as the instance
#define SET_TEXT L"SetText"

//bool ui:GetState()
//Returns whether checkbox is in checked state. If element has no state (if called for anything except checkbox) throws an error.
#define GET_STATE L"GetState"

//void ui:AddItem(string item)
//Adds a new item to the element. If element doesn't support items throws an error (only combobox, list and radiogroup do).
#define ADD_ITEM L"AddItem"

//void ui:DeleteItem(long index)
//Removes an item by given index. If element doesn't support items throws an error (only combobox, list and radiogroup do).
//If called without instance, root element will be used as the instance.
#define DELETE_ITEM L"DeleteItem"

//long ui:GetSelectedIndex()
//Returns the currently selected item. If element doesn't support items throws an error (only combobox, list and radiogroup do).
#define GET_SELECTED_INDEX L"GetSelectedIndex"

//void ui:SetSelectedIndex(long index)
//Makes item by given index selected. If element doesn't support items throws an error (only combobox, list and radiogroup do).
//If called without instance, root element will be used as the instance.
#define SET_SELECTED_INDEX L"SetSelectedIndex"

//long ui:GetItemsCount()
//Returns a number of items element has. If element doesn't support items throws an error (only combobox, list and radiogroup do).
#define GET_ITEMS_COUNT L"GetItemsCount"

//UI ui:GetItem(long index)
//Returns an item by given index. If element doesn't support items throws an error (only combobox, list and radiogroup do). If index provided is outside valid range throws an error.
#define GET_ITEM L"GetItem"

//void ui:ClearItems()
//Removes all items from the element. If element doesn't support items throws an error (only combobox, list and radiogroup do).
#define CLEAR_ITEMS L"ClearItems"

//void UI:Get()
//Returns the root UI element. Doesn't require instance.
#define GET L"Get"

//void ui:ClearChildren()
//Removes all the children element has. If called without instance, root element will be used as the instance.
#define CLEAR_CHILDREN L"ClearChildren"

//void ui:ClearChildren(string name)
//Removes the child by a given name. If no such child present does nothing. If called without instance, root element will be used as the instance.
#define DELETE_CHILD L"DeleteChild"

//void ui:SetOnChangeCallback(string callbackName)
//Sets the callback that will be called when selected element is changed. If element doesn't support items throws an error (only combobox, list and radiogroup do). 
#define SET_ON_CHANGE_CALLBACK L"SetOnChangeCallback"

//void ui:SetOnClickCallback(string callbackName)
//Sets the callback that will be called when the button is clicked. If element is not a button throws an error. 
#define SET_ON_CLICK_CALLBACK L"SetOnClickCallback"

//void ui:SetBackgroundImage(string image)
//Sets the background image to the button. If target element is not a button throws an error.
#define SET_BACKGROUND_IMAGE L"SetBackgroundImage"

//void ui:ApplyTheme(string pathToTheme)
//Loads and applies a theme to the element and all its children. If no such theme is present the default theme will be applied. If called without instance, root element will be used as the instance.
#define APPLY_THEME L"ApplyTheme"

//void UI:SetTargetSize(int width, int height)
//Sets the native size of UI that will then be adjusted for current resolution
#define SET_TARGET_SIZE L"SetTargetSize"

//void UI:SetScale(float scale)
//Sets the scale of UI elements that don't adjust to resolution, like borders, line sizes, etc. Also affects font size
#define SET_SCALE L"SetScale"

/*PROPERTIES*/

//Gets or sets whether the element is visible. Invisible elements don't displayed on the screen and don't receive input events. If called without instance, root element will be used as the instance
#define PROPERTY_VISIBLE L"visible"

//Gets or sets the element's text. If element has no text (for example, panel) throws an error. If element has multiple text items (example: combobox) returns the currently selected item. If called without instance, root element will be used as the instance
#define PROPERTY_TEXT L"text"

//Returns whether checkbox is in checked state. If element has no state (if called for anything except checkbox) throws an error.
#define PROPERTY_STATE L"state"

//Returns the x coordinate of an element. Note that this coordinate is relative to its parent.
#define PROPERTY_X L"x"

//Returns the y coordinate of an element. Note that this coordinate is relative to its parent.
#define PROPERTY_Y L"y"

//Returns the width of an element.
#define PROPERTY_WIDTH L"width"

//Returns the height of an element.
#define PROPERTY_HEIGHT L"height"

/*FUNCTIONS*/
//void MessageBox(string text, string caption)
//Displays message box with specified text and caption
#define MESSAGE_BOX L"MessageBox"
