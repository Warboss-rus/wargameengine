#include "UIConfig.h"
#include <GL\glut.h>

std::string CUIConfig::texture = "g2Default.png";
void * CUIConfig::font = GLUT_BITMAP_HELVETICA_18;
int CUIConfig::fontHeight = 18;
CUIConfig::sButton CUIConfig::button;
CUIConfig::sListBox CUIConfig::listbox;
CUIConfig::sCheckBox CUIConfig::checkbox;
float CUIConfig::defaultColor[3] = {0.6f, 0.6f, 0.6f};
float CUIConfig::textfieldColor[3] = {1.0f, 1.0f, 1.0f};
float CUIConfig::textColor[3] = {0.0f, 0.0f, 0.0f};

CUIConfig::sButton::sButton()
{
	texCoord[0] = 0.02344;
	texCoord[1] = 0.59375;
	texCoord[2] = 0.22266;
	texCoord[3] = 0.53516;
	pressedTexCoord[0] = 0.02344;
	pressedTexCoord[1] = 0.66016;
	pressedTexCoord[2] = 0.22266;
	pressedTexCoord[3] = 0.60157;
}

CUIConfig::sListBox::sListBox()
{
	texCoord[0] = 0.27734;
	texCoord[1] = 0.80469;
	texCoord[2] = 0.3125;
	texCoord[3] = 0.74219;
	expandedTexCoord[0] = 0.27734;
	expandedTexCoord[1] = 0.74219;
	expandedTexCoord[2] = 0.3125;
	expandedTexCoord[3] = 0.80469;
	buttonWidthCoeff = 0.66;
	borderSize = 2;
}

CUIConfig::sCheckBox::sCheckBox()
{
	texCoord[0] = 0.121;
	texCoord[1] = 0.789;
	texCoord[2] = 0.168;
	texCoord[3] = 0.7422;
	checkedTexCoord[0] = 0.121;
	checkedTexCoord[1] = 0.844;
	checkedTexCoord[2] = 0.168;
	checkedTexCoord[3] = 0.797;
	checkboxSizeCoeff = 1.0;
}