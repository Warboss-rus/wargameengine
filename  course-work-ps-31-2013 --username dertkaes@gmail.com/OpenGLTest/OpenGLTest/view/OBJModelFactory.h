#include <string>
#include "3dModel.h"

class CObjModelCreator
{
public:
	static C3DModel * Create(std::string const& path);
};