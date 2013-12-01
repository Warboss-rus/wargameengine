#include "UIText.h"
#include <GL\glut.h>
#include "UITheme.h"

void PrintText(int x, int y, int width, int height, std::string const& str, CUITheme::sText const& theme)
{
	if(theme.aligment == theme.center)
		x = (width - glutBitmapLength(theme.font, (unsigned char*)str.c_str())) / 2;
	if(theme.aligment == theme.right)
		x = width - glutBitmapLength(theme.font, (unsigned char*)str.c_str());
	y += (height - theme.fontHeight) / 2 + theme.fontHeight;
	glColor3f(theme.color[0], theme.color[1], theme.color[2]);
	PrintText(x, y, str, theme.font);
	glColor3f(0.0f, 0.0f, 0.0f);
}

void PrintText(int x, int y, std::string const& str, void* font)
{
	glRasterPos2i( x, y); // location to start printing text
	for(size_t i = 0; i < str.size(); i++) // loop until i is greater then l
	{
		glutBitmapCharacter(font, str[i]); // Print a character on the screen
	}
}