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

void WindowCoordsToWorldCoords(int windowX, int windowY, double & worldX, double & worldY, double worldZ = 0)
{
	double startx, starty, startz, endx, endy, endz;
	WindowCoordsToWorldVector(windowX, windowY, startx, starty, startz, endx, endy, endz);
	double a = (worldZ - startz) / (endz - startz);
	worldX = a * (endx - startx) + startx;
	worldY = a * (endy - starty) + starty;
}