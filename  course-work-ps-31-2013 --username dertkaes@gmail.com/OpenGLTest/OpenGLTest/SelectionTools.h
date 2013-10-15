class IObject;

bool BoxRayIntersect(double minB[3], double maxB[3], double origin[3], double dir[3]);
void WindowCoordsToWorldVector(int x, int y, double & startx, double & starty, double & startz, double & endx, double & endy, double & endz);
void DrawSelectionBox(const double * box);
void WindowCoordsToWorldCoords(int windowX, int windowY, double & worldX, double & worldY);
void RollDice(unsigned int number = 1, unsigned int sides = 6, bool groupByResult = false);