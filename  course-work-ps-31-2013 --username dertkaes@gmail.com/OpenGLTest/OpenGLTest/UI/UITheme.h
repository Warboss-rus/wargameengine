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
		void * font;
		int fontHeight;
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
		double texCoord[4];
		double pressedTexCoord[4];
	};
	sButton button;
	struct sListBox
	{
		double texCoord[4];
		double expandedTexCoord[4];
		double buttonWidthCoeff;
		int borderSize;
		int spacing;
	};
	sListBox listbox;
	struct sCheckBox
	{
		double texCoord[4];
		double checkedTexCoord[4];
		double checkboxSizeCoeff;
	};
	sCheckBox checkbox;
	struct sEdit
	{
		int borderSize;
	};
	sEdit edit;
	static CUITheme defaultTheme;
};