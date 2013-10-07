#include "Selection.h"
#include <GL\glut.h>
#include "ObjectInterface.h"
#include "ObjectManager.h"

void DrawSelectionBox(IObject * object)
{
	glPushMatrix();
	glTranslated(object->GetX(), object->GetY(), object->GetZ());
	glRotated(object->GetRotation(), 0.0, 0.0, 1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3d(0.0, 0.0, 255.0);
	const double * box = object->GetBounding();
	glBegin(GL_QUADS);
	//Left
	glVertex3d(box[3], box[1], box[2]);
	glVertex3d(box[3], box[4], box[2]);
	glVertex3d(box[3], box[4], box[5]);
	glVertex3d(box[3], box[1], box[5]);
	//Right
	glVertex3d(box[0], box[1], box[2]);
	glVertex3d(box[0], box[4], box[2]);
	glVertex3d(box[0], box[4], box[5]);
	glVertex3d(box[0], box[1], box[5]);
	//Front
	glVertex3d(box[0], box[1], box[2]);
	glVertex3d(box[3], box[1], box[2]);
	glVertex3d(box[3], box[1], box[5]);
	glVertex3d(box[0], box[1], box[5]);
	//Back
	glVertex3d(box[0], box[4], box[2]);
	glVertex3d(box[3], box[4], box[2]);
	glVertex3d(box[3], box[4], box[5]);
	glVertex3d(box[0], box[4], box[5]);
	glEnd();
	glColor3d(255.0, 255.0, 255.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();
}

IObject* SelectObject(int x, int y, CObjectManager const& objectManager)
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
	double startx, starty, startz, endx, endy, endz;
	//Cast a ray from eye to mouse cursor;
	gluUnProject(winX, winY, 0.0, matModelView, matProjection, 
             viewport, &startx, &starty, &startz); 
	gluUnProject(winX, winY, 1.0, matModelView, matProjection, 
             viewport, &endx, &endy, &endz);
	return objectManager.GetNearestObjectByVector(startx, starty, startz, endx, endy, endz);
}