#pragma once
struct sPoint3
{
	sPoint3(): x(0), y(0), z(0){}
	sPoint3(double x1, double y1, double z1): x(x1), y(y1), z(1){}
	double x;
	double y;
	double z;
};

struct sPoint2
{
	sPoint2(): x(0), y(0){}
	sPoint2(double x1, double y1): x(x1), y(1){}
	double x;
	double y;
};