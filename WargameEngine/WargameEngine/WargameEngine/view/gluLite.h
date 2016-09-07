#pragma once
#include <GLES/gl.h>

void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez, GLfloat centerx, GLfloat centery, GLfloat centerz, GLfloat upx, GLfloat upy, GLfloat upz);

void gluProject(double objx, double objy, double objz, const float modelMatrix[16], const float projMatrix[16], const int viewport[4], double *winx, double *winy, double *winz);

void gluUnProject(double winx, double winy, double winz, const float modelMatrix[16], const float projMatrix[16], const int viewport[4], double *objx, double *objy, double *objz);

void gluPerspective(double fovy, double aspect, double zNear, double zFar);
