#include "Application.h"
#include "LogWriter.h"
#include "Module.h"
#include "view/BuiltInImageReaders.h"
#include "view/OBJModelFactory.h"
#include "view/ColladaModelFactory.h"
#include "view/WBMModelFactory.h"
#include "impl/AssimpModelLoader.h"
#include <cstring>
#include <time.h>
#ifdef DIRECTX
#include "impl/GameWindowDirectX.h"
#define WINDOW_CLASS CGameWindowDirectX
#elif GLFW
#include "impl/GameWindowGLFW.h"
#define WINDOW_CLASS CGameWindowGLFW
#else
#include "impl/GameWindowGLUT.h"
#define WINDOW_CLASS CGameWindowGLUT
#endif
#include "impl/SoundPlayerFMod.h"
#include "impl/FreeTypeTextRasterizer.h"
#include "impl/ScriptHandlerLua.h"
#include "impl/PhysicsEngineBullet.h"
#include "impl/NetSocket.h"
#include "impl/AssimpModelLoader.h"
#include "impl/PathfindingMicroPather.h"
#include "Utils.h"
#include "view/PluginModelLoader.h"
#include "impl/Plugin.h"

using namespace wargameEngine;
using namespace view;

std::unique_ptr<IPlugin> TryLoadPlugin(const Path& path)
{
	try
	{
		return std::make_unique<Plugin>(path);
	}
	catch (...)
	{
	}
	return nullptr;
}

#ifdef _WINDOWS
#include <windows.h>

int WINAPI WinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*lpCmdLine*/, _In_ int /*nShowCmd*/)
{
	int argc = __argc;
	char** argv = __argv;
#else
int main(int argc, char* argv[])
{
#endif
	srand(static_cast<unsigned int>(time(NULL)));
	Context context;
	Module module;
	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "-module"))
		{
			i++;
			if (i == argc)
			{
				LogWriter::WriteLine("Module filename expected");
				return 1;
			}
			module = Module(Utf8ToWstring(argv[i]));
		}
	}
	if (module.name.empty())
	{
		module.script = L"main.lua";
		module.textures = L"texture\\";
		module.models = L"models\\";
	}
	context.window = std::make_unique<WINDOW_CLASS>();
	context.soundPlayer = std::make_unique<CSoundPlayerFMod>();
	context.textRasterizer = std::make_unique<FreeTypeTextRasterizer>();
	context.physicsEngine = std::make_unique<CPhysicsEngineBullet>();
	context.scriptHandler = std::make_unique<CScriptHandlerLua>();
	context.pathFinder = std::make_unique<CPathfindingMicroPather>();
	context.socketFactory = []() {
		return std::make_unique<CNetSocket>();
	};
	context.imageReaders.push_back(std::make_unique<CBmpImageReader>());
	context.imageReaders.push_back(std::make_unique<CTgaImageReader>());
	context.imageReaders.push_back(std::make_unique<CDdsImageReader>());
	context.imageReaders.push_back(std::make_unique<CStbImageReader>());
	context.modelReaders.push_back(std::make_unique<CObjModelFactory>());
	context.modelReaders.push_back(std::make_unique<CColladaModelFactory>());
	context.modelReaders.push_back(std::make_unique<CWBMModelFactory>());
	auto assimpPlugin = TryLoadPlugin("AssimpPlugin.dll");
	if (assimpPlugin)
	{
		context.modelReaders.push_back(std::make_unique<PluginModelLoader>(std::move(assimpPlugin)));
	}

	Application app(std::move(context));
	app.Run(std::move(module));
	return 0;
}
