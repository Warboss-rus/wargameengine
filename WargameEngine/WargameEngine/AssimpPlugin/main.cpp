#ifdef WIN32
#define PLUGIN_API __declspec(dllexport)
#endif
#include "..\WargameEngine\impl\AssimpModelLoader.h"

extern "C" {
	PLUGIN_API const char* GetType()
	{
		return "model";
	}

	PLUGIN_API wargameEngine::view::IModelReader * New()
	{
		return new CAssimpModelLoader();
	}

	PLUGIN_API void Delete(wargameEngine::view::IModelReader * reader)
	{
		delete reader;
	}
}
