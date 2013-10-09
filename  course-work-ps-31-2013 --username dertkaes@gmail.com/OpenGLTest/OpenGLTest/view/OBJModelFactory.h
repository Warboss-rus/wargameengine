#include "ModelFactoryInterface.h"

class CObjModelCreator : public IModelCreator
{
public:
	C3DModel * Create(std::string const& path);
};