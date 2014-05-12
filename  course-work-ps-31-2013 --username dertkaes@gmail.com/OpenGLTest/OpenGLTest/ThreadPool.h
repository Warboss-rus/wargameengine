#ifdef _WINDOWS
#include "ThreadPoolWindows.h"//WinAPI version
typedef CThreadPoolWindows CThreadPool;
#elif ___unix___
#include "ThreadPoolPOSIX.h"
typedef CThreadPoolPOSIX CThreadPool;
#else
#include "ThreadPoolPlaceHolder.h"//Singlethread version, need POSIX version
typedef CThreadPoolPlaceHolder CThreadPool;
#endif