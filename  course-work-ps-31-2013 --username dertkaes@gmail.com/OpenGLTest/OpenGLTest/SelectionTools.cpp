#include <GL\glut.h>
#include "ObjectInterface.h"
#include <vector>

void WindowCoordsToWorldVector(int x, int y, double & startx, double & starty, double & startz, double & endx, double & endy, double & endz)
{
	//Get model, projection and viewport matrices
	double matModelView[16], matProjection[16]; 
	int viewport[4]; 
	glGetDoublev( GL_MODELVIEW_MATRIX, matModelView ); 
	glGetDoublev( GL_PROJECTION_MATRIX, matProjection ); 
	glGetIntegerv( GL_VIEWPORT, viewport ); 
	//Set OpenGL Windows coordinates
	double winX = (double)x; 
	double winY = viewport[3] - (double)y;
	
	//Cast a ray from eye to mouse cursor;
	gluUnProject(winX, winY, 0.0, matModelView, matProjection, 
             viewport, &startx, &starty, &startz); 
	gluUnProject(winX, winY, 1.0, matModelView, matProjection, 
             viewport, &endx, &endy, &endz);
}

void WindowCoordsToWorldCoords(int windowX, int windowY, double & worldX, double & worldY)
{
	double startx, starty, startz, endx, endy, endz;
	WindowCoordsToWorldVector(windowX, windowY, startx, starty, startz, endx, endy, endz);
	double a = (-startz) / (endz - startz);
	worldX = a * (endx - startx) + startx;
	worldY = a * (endy - starty) + starty;
}

void RollDice(unsigned int number, unsigned int sides, bool groupByResult)
{
	std::vector<unsigned int> groups;
	std::string raw;
	for(unsigned int i = 0; i < sides; ++i)
	{
		groups.push_back(0);
	}

	for(unsigned int i = 0; i < number; ++i)
	{
		unsigned int result = rand() % sides;
		if(groupByResult)
		{
			groups[result]++;
		}
		else
		{
			char temp[6] = "";
			itoa((int)result + 1, temp, 10);
			std::string stemp(temp);
			raw += stemp + ' ';
		}
	}
	if(groupByResult)
	{
		for(unsigned int i = 0; i < sides; ++i)
		{
			char temp[6] = "";
			itoa((int)i + 1, temp, 10);
			std::string stemp(temp);
			raw += stemp + ": ";
			itoa((int)groups[i], temp, 10);
			stemp = temp;
			raw += stemp + '\n';
		}
	}
	MessageBoxA(NULL, raw.c_str(), "Player rolls", 0);
}