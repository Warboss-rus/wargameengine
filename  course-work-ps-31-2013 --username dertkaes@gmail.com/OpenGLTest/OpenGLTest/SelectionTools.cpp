#include "SelectionTools.h"
#include <GL\glut.h>
#include "ObjectInterface.h"
#include <vector>

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

void DrawSelectionBox(const double * box, double x, double y, double z, double rotation)
{
	glPushMatrix();
	glColor3d(0.0, 0.0, 255.0);
	//Left
	glBegin(GL_LINE_LOOP);
	glVertex3d(box[3] + x, box[1] + y, box[2] + z);
	glVertex3d(box[3] + x, box[4] + y, box[2] + z);
	glVertex3d(box[3] + x, box[4] + y, box[5]+ z);
	glVertex3d(box[3] + x, box[1] + y, box[5]+ z);
	glEnd();
	//Right
	glBegin(GL_LINE_LOOP);
	glVertex3d(box[0] + x, box[1] + y, box[2] + z);
	glVertex3d(box[0] + x, box[4] + y, box[2] + z);
	glVertex3d(box[0] + x, box[4] + y, box[5]+ z);
	glVertex3d(box[0] + x, box[1] + y, box[5]+ z);
	glEnd();
	//Front
	glBegin(GL_LINE_LOOP);
	glVertex3d(box[0] + x, box[1] + y, box[2] + z);
	glVertex3d(box[3] + x, box[1] + y, box[2] + z);
	glVertex3d(box[3] + x, box[1] + y, box[5]+ z);
	glVertex3d(box[0] + x, box[1] + y, box[5]+ z);
	glEnd();
	//Back
	glBegin(GL_LINE_LOOP);
	glVertex3d(box[0] + x, box[4] + y, box[2] + z);
	glVertex3d(box[3] + x, box[4] + y, box[2] + z);
	glVertex3d(box[3] + x, box[4] + y, box[5]+ z);
	glVertex3d(box[0] + x, box[4] + y, box[5]+ z);
	glEnd();
	glColor3d(255.0, 255.0, 255.0);
	glPopMatrix();
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