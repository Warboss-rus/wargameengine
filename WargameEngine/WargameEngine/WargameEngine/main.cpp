#include "view/GameView.h"
#include "LogWriter.h"
#include "Module.h"
#include "UI/UIElement.h"
#include <cstring>
#include "OSSpecific.h"
#include <time.h>
#ifdef _WINDOWS
#include <windows.h>
#include "Plugin.h"

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
	CGameView::GetInstance();
	return 0;
}
