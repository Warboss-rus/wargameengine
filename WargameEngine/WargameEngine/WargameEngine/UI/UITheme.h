#include <string>
#pragma once

class CUITheme
{
public:
	CUITheme();
	void Load(std::string const& filename);
	std::string texture;
	float defaultColor[4];
	float textfieldColor[4];
	struct sText
	{
		float color[4];
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
        sComboBox combobox;
	struct sList
	{
		int borderSize;
		int elementSize;
		float selectionColor[4];
		sText text;
	};
	sList list;
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
		float selectionColor[4];
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
		int buttonSize;
	};
	sScrollBar sbar;
	struct sWindow
	{
		int headerHeight;
		int buttonSize;
		float closeButtonTexCoord[4];
		sText headerText;
	};
	sWindow window;
	static CUITheme defaultTheme;
};