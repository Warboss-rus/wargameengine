class IObject;

void WindowCoordsToWorldVector(int x, int y, double & startx, double & starty, double & startz, double & endx, double & endy, double & endz);
void WindowCoordsToWorldCoords(int windowX, int windowY, double & worldX, double & worldY, double worldZ = 0);
void RollDice(unsigned int number = 1, unsigned int sides = 6, bool groupByResult = false);