#include "OBJModelFactory.h"
#include "WBMSerializer.h"

int main(int argc, char* argv[])
{
	if (argc != 2) return 1;
	std::string path(argv[1]);
	C3DModel * model;
	if (path.substr(path.find_last_of('.') + 1) == "dae")
	{
		model = LoadColladaModel(path);
	}
	else
	{
		model = LoadObjModel(path);
	}
	
	path = path.substr(0, path.find_last_of('.')) + ".wbm";
	SerializeToWBM(model, path);
	return 0;
}