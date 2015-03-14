# UI #

**UI** interface contains everything that you need to manage User Interface. WargameEngine UI supports following UI elements:
  * **[Button](UIButton.md)**
  * **[CheckBox](UICheckBox.md)**
  * **[ComboBox](UIComboBox.md)**
  * **[Edit](UIEdit.md)**
  * **[List](UIList.md)**
  * **[Panel](UIPanel.md)**
  * **[RadioGroup](UIRadioGroup.md)**
  * **[StaticText](UIStaticText.md)**
  * **Abstract UI element** has basic UI functionality. Cannot be created. Used to represent a screenspace.
All UI elements may have children. The position of UI element is measured from its parent. The only element that has no parent is a screenspace.
For example, we have a panel with coordinates (20, 10) that has a child button with coordinates (5, 5) so it has screenspace coordinates of (25, 15).
If we move the panel to (30, 15) then button screenspace coordinates will be (35, 20).
The element position and size can also be modified when a screenspace size has been changed. In this case all elements are scaled to match a new screenspace
size.

# Functions #

  * **[NewButton](LuaUINewButton.md)** adds a new child button to this element.
  * **[NewStaticText](LuaUINewStaticText.md)** adds a new child static text to this element.
  * **[NewPanel](LuaUINewPanel.md)** adds a new child panel to this element.
  * **[NewCheckbox](LuaUINewCheckbox.md)** adds a new child checkBox to this element.
  * **[NewCombobox](LuaUINewCombobox.md)** adds a new child comboBox to this element.
  * **[NewEdit](LuaUINewEdit.md)** adds a new child edit to this element.
  * **[NewList](LuaUINewList.md)** adds a new child list to this element.
  * **[NewRadioGroup](LuaUINewRadioGroup.md)** adds a new child radiogroup to this element.
  * **[GetChild](LuaUIGetChild.md)** retrives a child element by name.
  * **[SetVisible](LuaUISetVisible.md)** sets element visibility.
  * **[GetVisible](LuaUIGetVisible.md)** returns element visibility.
  * **[GetText](LuaUIGetText.md)** return element text. Returns empty string in case of **Abstract UI element** or **[Panel](UIPanel.md)**.
  * **[SetText](LuaUISetText.md)** sets element text. Has no effect to **Abstract UI element** or **[Panel](UIPanel.md)**.
  * **[GetState](LuaUIGetState.md)** returns if element is checked. Must only be called with **[CheckBoxes](UICheckBox.md)**.
  * **[AddItem](LuaUIAddItem.md)** adds a new item to list. Works only with **[ComboBox](UIComboBox.md)**, **[List](UIList.md)** or **[RadioGroup](UIRadioGroup.md)**, in other cases does nothing.
  * **[DeleteItem](LuaUIDeleteItem.md)** delete item from list by index. Works only with **[ComboBox](UIComboBox.md)**, **[List](UIList.md)** or **[RadioGroup](UIRadioGroup.md)**, in other cases does nothing.
  * **[GetSelectedIndex](LuaUIGetSelectedIndex.md)** returns an index of item in the list that is currently selected. Works only with **[ComboBox](UIComboBox.md)**, **[List](UIList.md)** or **[RadioGroup](UIRadioGroup.md)**, in other cases returns -1.
  * **[SetSelectedIndex](LuaUISetSelectedIndex.md)** sets the selected item by index. Works only with **[ComboBox](UIComboBox.md)**, **[List](UIList.md)** or **[RadioGroup](UIRadioGroup.md)**, in other cases does nothing.
  * **[GetItemsCount](LuaUIGetItemsCount.md)** returns a number of items in the list. Works only with **[ComboBox](UIComboBox.md)**, **[List](UIList.md)** or **[RadioGroup](UIRadioGroup.md)**, in other cases returns 0.
  * **[GetItem](LuaUIGetItem.md)** returns an item in the list by index. Works only with **[ComboBox](UIComboBox.md)**, **[List](UIList.md)** or **[RadioGroup](UIRadioGroup.md)**, in other cases returns empty string.
  * **[ClearItems](LuaUIClearItems.md)** removes all items from the list. Works only with **[ComboBox](UIComboBox.md)**, **[List](UIList.md)** or **[RadioGroup](UIRadioGroup.md)**, in other cases does nothing.
  * **[Get](LuaUIGet.md)** retrieves the screenspace. The only function that suppose to be called from the class itself, not its instance.
  * **[ClearChildren](LuaUIClearChildren.md)** removes all element's children.
  * **[DeleteChild](LuaUIDeleteChild.md)** removes an element's child by name. If an element has no such child then nothing happens.
  * **[SetOnChangeCallback](LuaUISetOnChangeCallback.md)** sets a callback that triggers when **[CheckBox](UICheckBox.md)** is checked or an item from the **[ComboBox](UIComboBox.md)**, **[List](UIList.md)** or **[RadioGroup](UIRadioGroup.md)** is selected. In other cases does nothing.
