#include <math.h>
class IMoveLimiter
{
public:
	virtual bool CheckPosition(double x, double y, double z, double rotation) = 0;
	virtual ~IMoveLimiter() {}
};

class CMoveLimiterCircle : public IMoveLimiter
{
public:
	CMoveLimiterCircle(double x, double y, double radius):m_x(x), m_y(y), m_radius(radius) {}
	bool CheckPosition(double x, double y, double z, double rotation) { return sqrt((x - m_x) * (x - m_x) + (y - m_y) * (y - m_y)) <= m_radius;}
private:
	double m_x;
	double m_y;
	double m_radius;
};

class CMoveLimiterRectangle : public IMoveLimiter
{
public:
	CMoveLimiterRectangle(double x1, double y1, double x2, double y2):m_minX((x1 < x2)?x1:x2), m_maxX((x1 > x2)?x1:x2), m_minY((y1 < y2)?y1:y2), m_maxY((y1 > y2)?y1:y2) {}
	bool CheckPosition(double x, double y, double z, double rotation) { return (x >= m_minX && x <= m_maxX && y >= m_minY && y <= m_maxY); }
private:
	double m_minX;
	double m_minY;
	double m_maxX;
	double m_maxY;
};

class CMoveLimiterStatic : public IMoveLimiter
{
public:
	bool CheckPosition(double x, double y, double z, double rotation) { return false; }
};