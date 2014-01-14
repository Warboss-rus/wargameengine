class IObject;

void WindowCoordsToWorldVector(int x, int y, float & startx, float & starty, float & startz, float & endx, float & endy, float & endz);
void WindowCoordsToWorldCoords(int windowX, int windowY, float & worldX, float & worldY, float worldZ = 0);