#include <string>
#pragma once

class CUITheme
{
public:
	CUITheme();
	std::string texture;
	float defaultColor[3];
	float textfieldColor[3];
	struct sText
	{
		float color[3];
		std::string font;
		unsigned int fontSize;
		enum eAligment
	{
		left = 0,
		center,
		right
	};
		eAligment aligment;
	};
	sText text;
	struct sButton
	{
	public:
		float texCoord[4];
		float pressedTexCoord[4];
		sText text;
	};
	sButton button;
	struct sComboBox
	{
		float texCoord[4];
		float expandedTexCoord[4];
		float buttonWidthCoeff;
		int borderSize;
		int elementSize;
		sText text;
	};
	struct sList
	{
		int borderSize;
		int elementSize;
		sText text;
	};
	sList list;
	sComboBox combobox;
	struct sCheckBox
	{
		float texCoord[4];
		float checkedTexCoord[4];
		float checkboxSizeCoeff;
		sText text;
	};
	sCheckBox checkbox;
	struct sEdit
	{
		int borderSize;
		sText text;
	};
	sEdit edit;
	struct sRadioGroup
	{
		float texCoord[4];
		float selectedTexCoord[4];
		float buttonSize;
		float elementSize;
		sText text;
	};
	sRadioGroup radiogroup;
	struct sScrollBar
	{
		float texCoord[4];
		float pressedTexCoord[4];
		int width;
		int buttonHeight;
	};
	sScrollBar sbar;
	static CUITheme defaultTheme;
};