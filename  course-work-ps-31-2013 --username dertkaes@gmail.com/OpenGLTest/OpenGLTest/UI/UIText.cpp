#include "UIText.h"
#include <GL\glut.h>
#include "UITheme.h"

void CUIText::Draw() const
{
	int x = m_x;
	if(m_theme.aligment == m_theme.center)
		x = (m_width - glutBitmapLength(m_theme.font, (unsigned char*)m_text.c_str())) / 2;
	if(m_theme.aligment == m_theme.right)
		x = m_width - glutBitmapLength(m_theme.font, (unsigned char*)m_text.c_str());
	glColor3f(m_theme.color[0], m_theme.color[1], m_theme.color[2]);
	PrintText(x, m_y, m_text, m_theme.font);
	glColor3f(0.0f, 0.0f, 0.0f);
}

void PrintText(int x, int y, std::string str, void* font)
{
	glRasterPos2i( x, y); // location to start printing text
	for(size_t i = 0; i < str.size(); i++) // loop until i is greater then l
	{
		glutBitmapCharacter(font, str[i]); // Print a character on the screen
	}
}