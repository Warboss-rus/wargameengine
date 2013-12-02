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
	text.font = GLUT_BITMAP_HELVETICA_18;
	text.fontHeight = 18;
	text.aligment = text.left;
	//button
	button.texCoord[0] = 0.02344;
	button.texCoord[1] = 0.59375;
	button.texCoord[2] = 0.22266;
	button.texCoord[3] = 0.53516;
	button.pressedTexCoord[0] = 0.02344;
	button.pressedTexCoord[1] = 0.66016;
	button.pressedTexCoord[2] = 0.22266;
	button.pressedTexCoord[3] = 0.60157;
	//listbox
	listbox.texCoord[0] = 0.27734;
	listbox.texCoord[1] = 0.80469;
	listbox.texCoord[2] = 0.3125;
	listbox.texCoord[3] = 0.74219;
	listbox.expandedTexCoord[0] = 0.27734;
	listbox.expandedTexCoord[1] = 0.74219;
	listbox.expandedTexCoord[2] = 0.3125;
	listbox.expandedTexCoord[3] = 0.80469;
	listbox.buttonWidthCoeff = 0.66;
	listbox.borderSize = 2;
	listbox.spacing = 2;
	//checkbox
	checkbox.texCoord[0] = 0.121;
	checkbox.texCoord[1] = 0.789;
	checkbox.texCoord[2] = 0.168;
	checkbox.texCoord[3] = 0.7422;
	checkbox.checkedTexCoord[0] = 0.121;
	checkbox.checkedTexCoord[1] = 0.844;
	checkbox.checkedTexCoord[2] = 0.168;
	checkbox.checkedTexCoord[3] = 0.797;
	checkbox.checkboxSizeCoeff = 1.0;
	edit.borderSize = 2;
}