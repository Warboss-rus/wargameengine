#include "UITheme.h"
#include <GL\glut.h>
#include <memory>

CUITheme CUITheme::defaultTheme;

CUITheme::CUITheme()
{
	texture = "g2Default.png";
	for(unsigned int i = 0; i < 3; ++i)
	{
		defaultColor[i] = 0.6f;
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
	combobox.spacing = 2;
	combobox.text = text;
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
	edit.borderSize = 2;
	edit.text = text;
}