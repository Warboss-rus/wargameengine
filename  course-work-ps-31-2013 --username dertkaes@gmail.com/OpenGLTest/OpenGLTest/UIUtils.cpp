#include <GL\glut.h>
#include "UIUtils.h"

void PrintText(int x, int y, std::string str, void* font)
{
	glRasterPos2i( x, y); // location to start printing text
	for(size_t i = 0; i < str.size(); i++) // loop until i is greater then l
	{
		glutBitmapCharacter(font, str[i]); // Print a character on the screen
	}
}