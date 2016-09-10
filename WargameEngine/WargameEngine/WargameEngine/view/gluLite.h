#pragma once

void gluLookAt(float eyex, float eyey, float eyez, float centerx, float centery, float centerz, float upx, float upy, float upz);

void gluProject(double objx, double objy, double objz, const float modelMatrix[16], const float projMatrix[16], const int viewport[4], double *winx, double *winy, double *winz);

void gluUnProject(double winx, double winy, double winz, const float modelMatrix[16], const float projMatrix[16], const int viewport[4], double *objx, double *objy, double *objz);

void gluPerspective(double fovy, double aspect, double zNear, double zFar);
