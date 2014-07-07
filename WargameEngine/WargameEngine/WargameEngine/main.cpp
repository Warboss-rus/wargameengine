#define GLEW_STATIC
#include <GL/glew.h>
#include "view\GameView.h"
#include "LogWriter.h"
#include "Module.h"
#include "UI\UIElement.h"
#ifdef _WINDOWS
#include <windows.h>
#include "Task.h"
	
void ChangeDir(std::string const& path)
{
	SetCurrentDirectoryA(path.c_str());
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int argc = __argc;
	char** argv = __argv;
#else

void ChangeDir(std::string const& path)
{
	chdir(path.c_str())
}

int main(int argc, char* argv[])
{
#endif
	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i], "-module"))
		{
			i++;
			if (i == argc)
			{
				CLogWriter::WriteLine("Module filename expected");
				return 1;
			}
			sModule::Load(argv[i]);
			ChangeDir(sModule::folder);
			
		}
	}
	if (sModule::name.empty())
	{
		sModule::script = "main.lua";
		sModule::textures = "texture\\";
		sModule::models = "models\\";
	}
	CGameView::GetIntanse();
	return 0;
}
