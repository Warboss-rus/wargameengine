#include "view/gl.h"
#include "model\ObjectInterface.h"
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

void WindowCoordsToWorldVector(int x, int y, float & startx, float & starty, float & startz, float & endx, float & endy, float & endz)
{
	double dstartx, dstarty, dstartz, dendx, dendy, dendz; 
	
	WindowCoordsToWorldVector(x, y, dstartx, dstarty, dstartz, dendx, dendy, dendz);

	startx = (float)dstartx;
	starty = (float)dstarty;
	startz = (float)dstartz;
	endx = (float)dendx;
	endy = (float)dendy;
	endz = (float)dendz;
}


void WindowCoordsToWorldCoords(int windowX, int windowY, float & worldX, float & worldY, float worldZ = 0)
{
	double startx, starty, startz, endx, endy, endz;
	WindowCoordsToWorldVector(windowX, windowY, startx, starty, startz, endx, endy, endz);
	double a = (worldZ - startz) / (endz - startz);
	worldX = a * (endx - startx) + startx;
	worldY = a * (endy - starty) + starty;
}