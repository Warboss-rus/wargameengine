#ifdef _WINDOWS
#include "ThreadPoolWindows.h"//WinAPI version
typedef CThreadPoolWindows CThreadPool;
#else
#include "ThreadPoolPlaceHolder.h"//Singlethread version, need POSIX version
typedef CThreadPoolPlaceHolder CThreadPool;
#endif