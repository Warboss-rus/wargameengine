class IObject;

void WindowCoordsToWorldVector(int x, int y, double & startx, double & starty, double & startz, double & endx, double & endy, double & endz);
void WindowCoordsToWorldCoords(int windowX, int windowY, double & worldX, double & worldY, double worldZ = 0);