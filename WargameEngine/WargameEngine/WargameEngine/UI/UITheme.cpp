#include "UITheme.h"
#include <memory>
#include <fstream>
#include "../LogWriter.h"
#include <algorithm>
#include <sstream>

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

void RemoveSpaces(std::string & line)
{
	while (line.back() == ' ' || line.back() == '\t') line.pop_back();
	line = line.substr(line.find_first_not_of(' '));
}

void ReadFloats(float * floats, int number, std::string const& str)
{
	std::stringstream ss(str);
	for (int i = 0; i < number; ++i)
	{
		ss >> floats[i];
	}
}

int ReadInt(std::string const& str)
{
	std::stringstream ss(str);
	int result;
	ss >> result;
	return result;
}

CUITheme::sText::eAligment ReadAligment(std::string const& str)
{
	if (str == "center") return CUITheme::sText::center;
	if (str == "right") return CUITheme::sText::right;
	return CUITheme::sText::left;
}

void CUITheme::Load(std::string const& filename)
{
	std::ifstream iFile(filename);
	std::string line;
	if (!iFile.good())
	{
		CLogWriter::WriteLine("Cannot load UI theme " + filename);
		return;
	}
	while (std::getline(iFile, line))
	{
		size_t commentpos = line.find_first_of(';');
		if (commentpos != line.npos) line = line.substr(0, commentpos - 1);
		size_t eqPos = line.find('=');
		if (eqPos == line.npos) continue;
		std::string key = line.substr(0, eqPos - 1);
		std::string value = line.substr(eqPos + 1);
		RemoveSpaces(key);
		RemoveSpaces(value);
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		if (key == "texture") texture = value;
		else if (key == "defaultcolor") ReadFloats(defaultColor, 3, value);
		else if (key == "textfieldcolor") ReadFloats(textfieldColor, 3, value);
		else if (key == "textcolor") ReadFloats(text.color, 3, value);
		else if (key == "textfont") text.font = value;
		else if (key == "textfontsize") text.fontSize = ReadInt(value);
		else if (key == "textaligment") text.aligment = ReadAligment(value);
		else if (key == "buttontexcoord") ReadFloats(button.texCoord, 4, value);
		else if (key == "buttonpressedtexcoord") ReadFloats(button.pressedTexCoord, 4, value);
		else if (key == "buttontextcolor") ReadFloats(button.text.color, 3, value);
		else if (key == "buttontextfont") button.text.font = value;
		else if (key == "buttontextfontsize") button.text.fontSize = ReadInt(value);
		else if (key == "buttontextaligment") button.text.aligment = ReadAligment(value);
		else if (key == "comboboxtexcoord") ReadFloats(combobox.texCoord, 4, value);
		else if (key == "comboboxexpandedtexcoord") ReadFloats(combobox.expandedTexCoord, 4, value);
		else if (key == "comboboxbuttonwidthcoeff") ReadFloats(&combobox.buttonWidthCoeff, 1, value);
		else if (key == "comboboxbordersize") combobox.borderSize = ReadInt(value);
		else if (key == "comboboxelementsize") combobox.elementSize = ReadInt(value);
		else if (key == "comboboxtextcolor") ReadFloats(combobox.text.color, 3, value);
		else if (key == "comboboxtextfont") combobox.text.font = value;
		else if (key == "comboboxtextfontsize") combobox.text.fontSize = ReadInt(value);
		else if (key == "comboboxtextaligment") combobox.text.aligment = ReadAligment(value);
		else if (key == "listbordersize") list.borderSize = ReadInt(value);
		else if (key == "listelementsize") list.elementSize = ReadInt(value);
		else if (key == "listtextcolor") ReadFloats(list.text.color, 3, value);
		else if (key == "listtextfont") list.text.font = value;
		else if (key == "listtextfontsize") list.text.fontSize = ReadInt(value);
		else if (key == "listtextaligment") list.text.aligment = ReadAligment(value);
		else if (key == "checkboxtexcoord") ReadFloats(checkbox.texCoord, 4, value);
		else if (key == "checkboxcheckedtexcoord") ReadFloats(checkbox.checkedTexCoord, 4, value);
		else if (key == "checkboxcheckboxsizecoeff") ReadFloats(&checkbox.checkboxSizeCoeff, 1, value);
		else if (key == "checkboxtextcolor") ReadFloats(list.text.color, 3, value);
		else if (key == "checkboxtextfont") list.text.font = value;
		else if (key == "checkboxtextfontsize") list.text.fontSize = ReadInt(value);
		else if (key == "checkboxtextaligment") list.text.aligment = ReadAligment(value);
		else if (key == "editbordersize") edit.borderSize = ReadInt(value);
		else if (key == "edittextcolor") ReadFloats(edit.text.color, 3, value);
		else if (key == "edittextfont") edit.text.font = value;
		else if (key == "edittextfontsize") edit.text.fontSize = ReadInt(value);
		else if (key == "edittextaligment") edit.text.aligment = ReadAligment(value);
		else if (key == "radiogrouptexcoord") ReadFloats(radiogroup.texCoord, 4, value);
		else if (key == "radiogroupselectedtexcoord") ReadFloats(radiogroup.selectedTexCoord, 4, value);
		else if (key == "radiogroupbuttonsize") ReadFloats(&radiogroup.buttonSize, 1, value);
		else if (key == "radiogroupelementsize") ReadFloats(&radiogroup.elementSize, 1, value);
		else if (key == "radiogrouptextcolor") ReadFloats(radiogroup.text.color, 3, value);
		else if (key == "radiogrouptextfont") radiogroup.text.font = value;
		else if (key == "radiogrouptextfontsize") radiogroup.text.fontSize = ReadInt(value);
		else if (key == "radiogrouptextaligment") radiogroup.text.aligment = ReadAligment(value);
		else if (key == "scrollbartexcoord") ReadFloats(sbar.texCoord, 4, value);
		else if (key == "scrollbarpressedtexcoord") ReadFloats(sbar.pressedTexCoord, 4, value);
		else if (key == "scrollbarwidth") sbar.width = ReadInt(value);
		else if (key == "scrollbarbuttonheight") sbar.buttonHeight = ReadInt(value);
	}
}