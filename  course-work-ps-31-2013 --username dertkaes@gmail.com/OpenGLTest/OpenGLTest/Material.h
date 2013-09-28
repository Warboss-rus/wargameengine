#include <string>

struct sMaterial
{
	sMaterial():beginIndex(0)
	{
		for(unsigned int i = 0; i < 3; ++i)
		{
			ambient[i] = 0.2;
			diffuse[i] = 0.8;
			specular[i] = 0.0;
		}
		shininess = 0.0;
	}
	double ambient[3];
	double diffuse[3];
	double specular[3];
	double shininess;
	unsigned int beginIndex;
};