#include "3dModel.h"

class IModelCreator
{
public:
	virtual C3DModel * Create(std::string const& path) = 0;
	virtual ~IModelCreator() {}
};