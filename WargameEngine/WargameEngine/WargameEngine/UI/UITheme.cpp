#include "UITheme.h"
#include <limits.h>
#include <string.h>
#include "../LogWriter.h"
#include "../rapidxml/rapidxml.hpp"
#include <fstream>
#include "../Utils.h"

using namespace std;
using namespace rapidxml;

CUITheme::CUITheme()
	: texture(make_path(L"g2Default.png"))
{
	button.text.aligment = Text::Aligment::center;
}

float atoff(const char * ch)
{
	return static_cast<float>(atof(ch));
}

void GetValues(float * array, const char* data, size_t max = UINT_MAX)
{
	char * fl = strtok((char*)data, " \n\t");
	for(size_t i = 0; (i < max) && fl; ++i)
	{
		for (size_t j = 0; j < strlen(fl); ++j)
		{
			if (fl[j] == ',') fl[j] = '.';
		}
		array[i] = atoff(fl);
		fl = strtok(NULL, " \n\t");
	}
}

void ParseTextTheme(xml_node<>* theme, CUITheme::Text & text)
{
	if (theme->first_attribute("color")) GetValues(text.color, (char*)theme->first_attribute("color"), 3);
	if (theme->first_attribute("font")) text.font = theme->first_attribute("font")->value();
	if (theme->first_attribute("fontSize")) text.fontSize = static_cast<unsigned int>(atoi(theme->first_attribute("fontSize")->value()));
	if (theme->first_attribute("aligment"))
	{
		std::string aligment = theme->first_attribute("aligment")->value();
		if (aligment == "center") text.aligment = CUITheme::Text::Aligment::center;
		if (aligment == "right") text.aligment = CUITheme::Text::Aligment::right;
		if (aligment == "left") text.aligment = CUITheme::Text::Aligment::left;
	}
}

void CUITheme::Load(const Path& filename)
{
	auto content = ReadFile(filename);
	std::unique_ptr<xml_document<>> doc = std::make_unique<xml_document<>>();
	doc->parse<0>(content.data());
	xml_node<>* theme = doc->first_node();
	if (!theme)
	{
		LogWriter::WriteLine(to_string(filename) + " is not a valid theme file");
		return;
	}
	if (theme->first_attribute("texture")) texture = make_path(theme->first_attribute("texture")->value());
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