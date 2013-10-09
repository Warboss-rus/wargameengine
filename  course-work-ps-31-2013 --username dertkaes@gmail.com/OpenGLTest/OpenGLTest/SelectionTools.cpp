#include "SelectionTools.h"
#include <GL\glut.h>
#include "ObjectInterface.h"

bool BoxRayIntersect(double minB[3], double maxB[3], double origin[3], double dir[3])
{
	double coord[3];
	char inside = true;
	char quadrant[3];
	register int i;
	int whichPlane;
	double maxT[3];
	double candidatePlane[3];

	/* Find candidate planes; this loop can be avoided if
   	rays cast all from the eye(assume perpsective view) */
	for (i=0; i<3; i++)
		if(origin[i] < minB[i]) {
			quadrant[i] = 1;
			candidatePlane[i] = minB[i];
			inside = false;
		}else if (origin[i] > maxB[i]) {
			quadrant[i] = 0;
			candidatePlane[i] = maxB[i];
			inside = false;
		}else	{
			quadrant[i] = 2;
		}

	/* Ray origin inside bounding box */
	if(inside)	{
		coord[0] = origin[0];
		coord[1] = origin[1];
		coord[2] = origin[2];
		return true;
	}

	/* Calculate T distances to candidate planes */
	for (i = 0; i < 3; i++)
		if (quadrant[i] != 2 && dir[i] !=0.)
			maxT[i] = (candidatePlane[i]-origin[i]) / dir[i];
		else
			maxT[i] = -1.;

	/* Get largest of the maxT's for final choice of intersection */
	whichPlane = 0;
	for (i = 1; i < 3; i++)
		if (maxT[whichPlane] < maxT[i])
			whichPlane = i;

	/* Check final candidate actually inside box */
	if (maxT[whichPlane] < 0.) return (false);
	for (i = 0; i < 3; i++)
		if (whichPlane != i) {
			coord[i] = origin[i] + maxT[whichPlane] *dir[i];
			if (coord[i] < minB[i] || coord[i] > maxB[i])
				return (false);
		} else {
			coord[i] = candidatePlane[i];
		}
	return true;				/* ray hits box */
}

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