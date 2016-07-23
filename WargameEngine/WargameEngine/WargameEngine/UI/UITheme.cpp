#include "UITheme.h"
#include <limits.h>
#include "../LogWriter.h"
#include "../rapidxml/rapidxml.hpp"
#include <fstream>
#include "../Utils.h"

using namespace std;
using namespace rapidxml;

CUITheme CUITheme::defaultTheme;

CUITheme::CUITheme()
{
	texture = L"g2Default.png";
	for(unsigned int i = 0; i < 3; ++i)
	{
		defaultColor[i] = 0.75f;
		textfieldColor[i] = 1.0f;
		text.color[i] = 0.0f;
	}
	defaultColor[3] = 1.0f;
	textfieldColor[3] = 1.0f;
	text.color[3] = 1.0f;
	//text
	text.font = "times.ttf";
	text.fontSize = 18;
	text.aligment = text.left;
	//button
	button.texCoord[0] = 0.02344f;
	button.texCoord[1] = 0.59375f;
	button.texCoord[2] = 0.22266f;
	button.texCoord[3] = 0.53516f;
	button.pressedTexCoord[0] = 0.02344f;
	button.pressedTexCoord[1] = 0.66016f;
	button.pressedTexCoord[2] = 0.22266f;
	button.pressedTexCoord[3] = 0.60157f;
	button.text = text;
	button.text.aligment = text.center;
	//combobox
	combobox.texCoord[0] = 0.27734f;
	combobox.texCoord[1] = 0.80469f;
	combobox.texCoord[2] = 0.3125f;
	combobox.texCoord[3] = 0.74219f;
	combobox.expandedTexCoord[0] = 0.27734f;
	combobox.expandedTexCoord[1] = 0.74219f;
	combobox.expandedTexCoord[2] = 0.3125f;
	combobox.expandedTexCoord[3] = 0.80469f;
	combobox.buttonWidthCoeff = 0.66f;
	combobox.borderSize = 2;
	combobox.elementSize = 25;
	combobox.text = text;
	//list
	list.borderSize = 2;
	list.elementSize = 20;
	list.text = text;
	list.selectionColor[0] = 0.2f;
	list.selectionColor[1] = 0.2f;
	list.selectionColor[2] = 1.0f;
	list.selectionColor[3] = 1.0f;
	//checkbox
	checkbox.texCoord[0] = 0.121f;
	checkbox.texCoord[1] = 0.789f;
	checkbox.texCoord[2] = 0.168f;
	checkbox.texCoord[3] = 0.7422f;
	checkbox.checkedTexCoord[0] = 0.121f;
	checkbox.checkedTexCoord[1] = 0.844f;
	checkbox.checkedTexCoord[2] = 0.168f;
	checkbox.checkedTexCoord[3] = 0.797f;
	checkbox.checkboxSizeCoeff = 1.0f;
	checkbox.text = text;
	//edit
	edit.borderSize = 2;
	edit.text = text;
	edit.selectionColor[0] = 0.0f;
	edit.selectionColor[1] = 0.0f;
	edit.selectionColor[2] = 1.0f;
	edit.selectionColor[3] = 1.0f;
	//radiogroup
	radiogroup.texCoord[0] = 0.379f;
	radiogroup.texCoord[1] = 0.793f;
	radiogroup.texCoord[2] = 0.429f;
	radiogroup.texCoord[3] = 0.742f;
	radiogroup.selectedTexCoord[0] = 0.379f;
	radiogroup.selectedTexCoord[1] = 0.8515f;
	radiogroup.selectedTexCoord[2] = 0.429f;
	radiogroup.selectedTexCoord[3] = 0.8f;
	radiogroup.buttonSize = 15.0f;
	radiogroup.elementSize = 22.0f;
	radiogroup.text = text;
	sbar.texCoord[0] = 0.07f;
	sbar.texCoord[1] = 0.984f;
	sbar.texCoord[2] = 0.117f;
	sbar.texCoord[3] = 0.953f;
	sbar.pressedTexCoord[0] = 0.121f;
	sbar.pressedTexCoord[1] = 0.984f;
	sbar.pressedTexCoord[2] = 0.168f;
	sbar.pressedTexCoord[3] = 0.953f;
	sbar.buttonSize = 20;
	sbar.width = 20;
	//window
	window.buttonSize = 20;
	window.headerHeight = 20;
	window.headerText = text;
	window.closeButtonTexCoord[0] = 0.6758f;
	window.closeButtonTexCoord[1] = 0.996f;
	window.closeButtonTexCoord[2] = 0.7343f;
	window.closeButtonTexCoord[3] = 0.9453f;
}

float atoff(const char * ch)
{
	return static_cast<float>(atof(ch));
}

void GetValues(float * array, const char* data, unsigned int max = UINT_MAX)
{
	char * fl = strtok((char*)data, " \n\t");
	unsigned int i = 0;
	while (fl != NULL && i < max)
	{
		for (size_t j = 0; j < strlen(fl); ++j)
		{
			if (fl[j] == ',') fl[j] = '.';
		}
		array[i] = atoff(fl);
		i++;
		fl = strtok(NULL, " \n\t");
	}
}

void ParseTextTheme(xml_node<>* theme, CUITheme::sText & text)
{
	if (theme->first_attribute("color")) GetValues(text.color, (char*)theme->first_attribute("color"), 3);
	if (theme->first_attribute("font")) text.font = theme->first_attribute("font")->value();
	if (theme->first_attribute("fontSize")) text.fontSize = static_cast<unsigned int>(atoi(theme->first_attribute("fontSize")->value()));
	if (theme->first_attribute("aligment"))
	{
		std::string aligment = theme->first_attribute("aligment")->value();
		if (aligment == "center") text.aligment = CUITheme::sText::center;
		if (aligment == "right") text.aligment = CUITheme::sText::right;
		if (aligment == "left") text.aligment = CUITheme::sText::left;
	}
}

void CUITheme::Load(std::wstring const& filename)
{
	auto content = ReadFile(filename);
	std::unique_ptr<xml_document<>> doc = std::make_unique<xml_document<>>();
	doc->parse<0>(content.data());
	xml_node<>* theme = doc->first_node();
	if (!theme)
	{
		LogWriter::WriteLine(filename + L" is not a valid theme file");
		return;
	}
	if (theme->first_attribute("texture")) texture = Utf8ToWstring(theme->first_attribute("texture")->value());
	if (theme->first_attribute("defaultColor")) GetValues(defaultColor, theme->first_attribute("defaultColor")->value(), 3);
	if (theme->first_attribute("textfieldColor")) GetValues(defaultColor, theme->first_attribute("textfieldColor")->value(), 3);
	//text block
	xml_node<>* themeText = theme->first_node("text");
	if (themeText) ParseTextTheme(themeText, text);
	//button block
	xml_node<>* themeButton = theme->first_node("button");
	if (themeButton)
	{
		if (themeButton->first_attribute("texCoord")) GetValues(button.texCoord, themeButton->first_attribute("texCoord")->value(), 4);
		xml_node<>* themePressed = themeButton->first_node("pressed");
		if (themePressed && themePressed->first_attribute("texCoord")) GetValues(button.pressedTexCoord, themePressed->first_attribute("texCoord")->value(), 4);
		themeText = themeButton->first_node("text");
		if (themeText) ParseTextTheme(themeText, button.text);
	}
	//combobox
	xml_node<>* themeCombobox = theme->first_node("combobox");
	if (themeCombobox)
	{
		if (themeCombobox->first_attribute("texCoord")) GetValues(combobox.texCoord, themeCombobox->first_attribute("texCoord")->value(), 4);
		if (themeCombobox->first_attribute("borderSize")) combobox.borderSize = atoi(themeCombobox->first_attribute("borderSize")->value());
		if (themeCombobox->first_attribute("elementSize")) combobox.elementSize = atoi(themeCombobox->first_attribute("elementSize")->value());
		xml_node<>* themeExpanded = themeCombobox->first_node("expanded");
		if (themeExpanded && themeExpanded->first_attribute("texCoord")) GetValues(combobox.expandedTexCoord, themeExpanded->first_attribute("texCoord")->value(), 4);
		themeText = themeCombobox->first_node("text");
		if (themeText) ParseTextTheme(themeText, combobox.text);
	}
	//list
	xml_node<>* themeList = theme->first_node("list");
	if (themeList)
	{
		if (themeList->first_attribute("borderSize")) list.borderSize = atoi(themeList->first_attribute("borderSize")->value());
		if (themeList->first_attribute("elementSize")) list.elementSize = atoi(themeList->first_attribute("elementSize")->value());
		themeText = themeList->first_node("text");
		if (themeText) ParseTextTheme(themeText, list.text);
	}
	//checkbox
	xml_node<>* themeCheckbox = theme->first_node("checkbox");
	if (themeCheckbox)
	{
		if (themeCheckbox->first_attribute("texCoord")) GetValues(checkbox.texCoord, themeCheckbox->first_attribute("texCoord")->value(), 4);
		if (themeCheckbox->first_attribute("sizeCoeff")) checkbox.checkboxSizeCoeff = atoff(themeCheckbox->first_attribute("sizeCoeff")->value());
		xml_node<>* themeChecked = themeCheckbox->first_node("checked");
		if (themeChecked && themeChecked->first_attribute("texCoord")) GetValues(checkbox.checkedTexCoord, themeChecked->first_attribute("texCoord")->value(), 4);
		themeText = themeCheckbox->first_node("text");
		if (themeText) ParseTextTheme(themeText, checkbox.text);
	}
	//edit
	xml_node<>* themeEdit = theme->first_node("list");
	if (themeEdit)
	{
		if (themeEdit->first_attribute("borderSize")) edit.borderSize = atoi(themeEdit->first_attribute("borderSize")->value());
		themeText = themeEdit->first_node("text");
		if (themeText) ParseTextTheme(themeText, edit.text);
	}
	//radiogroup
	xml_node<>* themeRadiogroup = theme->first_node("radiogroup");
	if (themeRadiogroup)
	{
		if (themeRadiogroup->first_attribute("texCoord")) GetValues(radiogroup.texCoord, themeRadiogroup->first_attribute("texCoord")->value(), 4);
		if (themeRadiogroup->first_attribute("buttonSize")) radiogroup.buttonSize = atoff(themeRadiogroup->first_attribute("buttonSize")->value());
		if (themeRadiogroup->first_attribute("elementSize")) radiogroup.elementSize = atoff(themeRadiogroup->first_attribute("elementSize")->value());
		xml_node<>* themeSelected = themeRadiogroup->first_node("selected");
		if (themeSelected && themeSelected->first_attribute("texCoord")) GetValues(radiogroup.selectedTexCoord, themeSelected->first_attribute("texCoord")->value(), 4);
		themeText = themeRadiogroup->first_node("text");
		if (themeText) ParseTextTheme(themeText, radiogroup.text);
	}
	//scrollbar
	xml_node<>* themeScrollbar = theme->first_node("scrollbar");
	if (themeScrollbar)
	{
		if (themeScrollbar->first_attribute("texCoord")) GetValues(sbar.texCoord, themeScrollbar->first_attribute("texCoord")->value(), 4);
		if (themeScrollbar->first_attribute("buttonHeight")) sbar.buttonSize = atoi(themeScrollbar->first_attribute("buttonHeight")->value());
		if (themeScrollbar->first_attribute("width")) sbar.width = atoi(themeScrollbar->first_attribute("width")->value());
		xml_node<>* themePressed = themeScrollbar->first_node("pressed");
		if (themePressed && themePressed->first_attribute("texCoord")) GetValues(sbar.pressedTexCoord, themePressed->first_attribute("texCoord")->value(), 4);
	}
	//window
	xml_node<>* themeWindow = theme->first_node("window");
	if (themeWindow)
	{
		if (themeWindow->first_attribute("headerHeight")) window.headerHeight = atoi(themeWindow->first_attribute("headerHeight")->value());
		if (themeWindow->first_attribute("buttonSize")) window.buttonSize = atoi(themeWindow->first_attribute("buttonSize")->value());
	}
	doc->clear();
}