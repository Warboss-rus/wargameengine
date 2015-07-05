#include "UITheme.h"
#include <limits.h>
#include "../LogWriter.h"
#include "../tinyxml.h"

CUITheme CUITheme::defaultTheme;

CUITheme::CUITheme()
{
	texture = "g2Default.png";
	for(unsigned int i = 0; i < 3; ++i)
	{
		defaultColor[i] = 0.75f;
		textfieldColor[i] = 1.0f;
		text.color[i] = 0.0f;
	}
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
	sbar.buttonHeight = 20;
	sbar.width = 20;
}

float atoff(const char * ch)
{
	return static_cast<float>(atof(ch));
}

void GetFloats(float * array, const char* data, unsigned int max = UINT_MAX)
{
	char * fl = strtok((char*)data, " \n\t");
	unsigned int i = 0;
	while (fl != NULL && i < max)
	{
		for (size_t i = 0; i < strlen(fl); ++i)
		{
			if (fl[i] == ',') fl[i] = '.';
		}
		array[i] = atoff(fl);
		i++;
		fl = strtok(NULL, " \n\t");
	}
}

void ParseTextTheme(TiXmlElement* theme, CUITheme::sText & text)
{
	if (theme->Attribute("color")) GetFloats(text.color, (char*)theme->Attribute("color"), 3);
	if (theme->Attribute("font")) text.font = theme->Attribute("font");
	if (theme->Attribute("fontSize")) text.fontSize = static_cast<unsigned int>(atoi(theme->Attribute("fontSize")));
	if (theme->Attribute("aligment"))
	{
		std::string aligment = theme->Attribute("aligment");
		if (aligment == "center") text.aligment = CUITheme::sText::center;
		if (aligment == "right") text.aligment = CUITheme::sText::right;
		if (aligment == "left") text.aligment = CUITheme::sText::left;
	}
}

void CUITheme::Load(std::string const& filename)
{
	TiXmlDocument doc;
	doc.LoadFile(filename);
	TiXmlElement* theme = doc.RootElement();
	if (!theme)
	{
		LogWriter::WriteLine(filename + " is not a valid theme file");
		return;
	}
	if (theme->Attribute("texture")) texture = theme->Attribute("texture");
	if (theme->Attribute("defaultColor")) GetFloats(defaultColor, theme->Attribute("defaultColor"), 3);
	if (theme->Attribute("textfieldColor")) GetFloats(defaultColor, theme->Attribute("textfieldColor"), 3);
	//text block
	TiXmlElement* themeText = theme->FirstChildElement("text");
	if (themeText) ParseTextTheme(themeText, text);
	//button block
	TiXmlElement* themeButton = theme->FirstChildElement("button");
	if (themeButton)
	{
		if (themeButton->Attribute("texCoord")) GetFloats(button.texCoord, themeButton->Attribute("texCoord"), 4);
		TiXmlElement* themePressed = themeButton->FirstChildElement("pressed");
		if (themePressed && themePressed->Attribute("texCoord")) GetFloats(button.pressedTexCoord, themePressed->Attribute("texCoord"), 4);
		themeText = themeButton->FirstChildElement("text");
		if (themeText) ParseTextTheme(themeText, button.text);
	}
	//combobox
	TiXmlElement* themeCombobox = theme->FirstChildElement("combobox");
	if (themeCombobox)
	{
		if (themeCombobox->Attribute("texCoord")) GetFloats(combobox.texCoord, themeCombobox->Attribute("texCoord"), 4);
		if (themeCombobox->Attribute("borderSize")) combobox.borderSize = atoi(themeCombobox->Attribute("borderSize"));
		if (themeCombobox->Attribute("elementSize")) combobox.elementSize = atoi(themeCombobox->Attribute("elementSize"));
		TiXmlElement* themeExpanded = themeCombobox->FirstChildElement("expanded");
		if (themeExpanded && themeExpanded->Attribute("texCoord")) GetFloats(combobox.expandedTexCoord, themeExpanded->Attribute("texCoord"), 4);
		themeText = themeCombobox->FirstChildElement("text");
		if (themeText) ParseTextTheme(themeText, combobox.text);
	}
	//list
	TiXmlElement* themeList = theme->FirstChildElement("list");
	if (themeList)
	{
		if (themeList->Attribute("borderSize")) list.borderSize = atoi(themeList->Attribute("borderSize"));
		if (themeList->Attribute("elementSize")) list.elementSize = atoi(themeList->Attribute("elementSize"));
		themeText = themeList->FirstChildElement("text");
		if (themeText) ParseTextTheme(themeText, list.text);
	}
	//checkbox
	TiXmlElement* themeCheckbox = theme->FirstChildElement("checkbox");
	if (themeCheckbox)
	{
		if (themeCheckbox->Attribute("texCoord")) GetFloats(checkbox.texCoord, themeCheckbox->Attribute("texCoord"), 4);
		if (themeCheckbox->Attribute("sizeCoeff")) checkbox.checkboxSizeCoeff = atoff(themeCheckbox->Attribute("sizeCoeff"));
		TiXmlElement* themeChecked = themeCheckbox->FirstChildElement("checked");
		if (themeChecked && themeChecked->Attribute("texCoord")) GetFloats(checkbox.checkedTexCoord, themeChecked->Attribute("texCoord"), 4);
		themeText = themeCheckbox->FirstChildElement("text");
		if (themeText) ParseTextTheme(themeText, checkbox.text);
	}
	//edit
	TiXmlElement* themeEdit = theme->FirstChildElement("list");
	if (themeEdit)
	{
		if (themeEdit->Attribute("borderSize")) edit.borderSize = atoi(themeEdit->Attribute("borderSize"));
		themeText = themeEdit->FirstChildElement("text");
		if (themeText) ParseTextTheme(themeText, edit.text);
	}
	//radiogroup
	TiXmlElement* themeRadiogroup = theme->FirstChildElement("radiogroup");
	if (themeRadiogroup)
	{
		if (themeRadiogroup->Attribute("texCoord")) GetFloats(radiogroup.texCoord, themeRadiogroup->Attribute("texCoord"), 4);
		if (themeRadiogroup->Attribute("buttonSize")) radiogroup.buttonSize = atoff(themeRadiogroup->Attribute("buttonSize"));
		if (themeRadiogroup->Attribute("elementSize")) radiogroup.elementSize = atoff(themeRadiogroup->Attribute("elementSize"));
		TiXmlElement* themeSelected = themeRadiogroup->FirstChildElement("selected");
		if (themeSelected && themeSelected->Attribute("texCoord")) GetFloats(radiogroup.selectedTexCoord, themeSelected->Attribute("texCoord"), 4);
		themeText = themeRadiogroup->FirstChildElement("text");
		if (themeText) ParseTextTheme(themeText, radiogroup.text);
	}
	//scrollbar
	TiXmlElement* themeScrollbar = theme->FirstChildElement("scrollbar");
	if (themeScrollbar)
	{
		if (themeScrollbar->Attribute("texCoord")) GetFloats(sbar.texCoord, themeButton->Attribute("texCoord"), 4);
		if (themeScrollbar->Attribute("buttonHeight")) sbar.buttonHeight = atoi(themeScrollbar->Attribute("buttonHeight"));
		if (themeScrollbar->Attribute("width")) sbar.width = atoi(themeScrollbar->Attribute("width"));
		TiXmlElement* themePressed = themeScrollbar->FirstChildElement("pressed");
		if (themePressed && themePressed->Attribute("texCoord")) GetFloats(sbar.pressedTexCoord, themePressed->Attribute("texCoord"), 4);
	}
	doc.Clear();
}