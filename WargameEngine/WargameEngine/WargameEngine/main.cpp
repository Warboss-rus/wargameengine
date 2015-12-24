#include "view/GameView.h"
#include "LogWriter.h"
#include "Module.h"
#include <cstring>
#include "OSSpecific.h"
#include <time.h>
#ifdef DIRECTX
#include "view\GameWindowDirectX.h"
#define WINDOW_CLASS CGameWindowDirectX
#elif GLFW
#include "view\GameWindowGLFW.h"
#define WINDOW_CLASS CGameWindowGLFW
#else
#include "view\GameWindowGLUT.h"
#define WINDOW_CLASS CGameWindowGLUT
#endif
#include "SoundPlayerFMod.h"
#include "view\TextWriter.h"
#include "ScriptHandlerLua.h"
#include "NetSocket.h"
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
			sModule::Load(argv[i]);
			ChangeWorkingDirectory(sModule::folder.c_str());
		}
	}
	if (sModule::name.empty())
	{
		sModule::script = "main.lua";
		sModule::textures = "texture\\";
		sModule::models = "models\\";
	}
	sGameViewContext context;
	context.window = std::make_unique<WINDOW_CLASS>();
	context.soundPlayer = std::make_unique<CSoundPlayerFMod>();
	context.textWriter = std::make_unique<CTextWriter>(context.window->GetRenderer());
	context.scriptHandlerFactory = []() {
		return std::make_unique<CScriptHandlerLua>();
	};
	context.socketFactory = []() {
		return std::make_unique<CNetSocket>();
	};

	CGameView view(&context);
	return 0;
}
