#include "view/GameView.h"
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
#include "impl/TextWriter.h"
#include "impl/ScriptHandlerLua.h"
#include "impl/PhysicsEngineBullet.h"
#include "impl/NetSocket.h"
#include "impl/AssimpModelLoader.h"
#include "impl/PathfindingMicroPather.h"
#include "Utils.h"
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
	sGameViewContext context;
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
			context.module.Load(Utf8ToWstring(argv[i]));
		}
	}
	if (context.module.name.empty())
	{
		context.module.script = L"main.lua";
		context.module.textures = L"texture\\";
		context.module.models = L"models\\";
	}
	context.window = std::make_unique<WINDOW_CLASS>();
	context.soundPlayer = std::make_unique<CSoundPlayerFMod>();
	context.textWriter = std::make_unique<CTextWriter>();
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
	context.modelReaders.push_back(std::make_unique<CAssimpModelLoader>());

	CGameView view(&context);
	return 0;
}
