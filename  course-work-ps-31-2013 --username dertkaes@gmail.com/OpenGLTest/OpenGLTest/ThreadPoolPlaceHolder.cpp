#include "ThreadPoolPlaceHolder.h"

void CThreadPoolPlaceHolder::RunFunc(void(*func)(void*), void* param)
{
	func(param);
}

void CThreadPoolPlaceHolder::RunFunc(void(*func)(void*), void* param, void(*doneCallback)())
{
	func(param);
	doneCallback();
}

void CThreadPoolPlaceHolder::RunFunc(void* (*func)(void*), void* param, void(*doneCallback)(void*))
{
	void* result = func(param);
	doneCallback(result);
}

void CThreadPoolPlaceHolder::AsyncReadFile(std::string const& path, void(*func)(void*, unsigned int, void*), void* param, void(*doneCallback)())
{
	FILE * file = fopen(path.c_str(), "rb");
	fseek(file, 0L, SEEK_END);
	unsigned int size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	unsigned char* data = new unsigned char[size];
	fread(data, 1, size, file);
	fclose(file);
	func(data, size, param);
	if(doneCallback)
		doneCallback();
}

void CThreadPoolPlaceHolder::AsyncReadFile(std::string const& path, void* (*func)(void*, unsigned int, void*), void* param, void(*doneCallback)(void*))
{
	FILE * file = fopen(path.c_str(), "rb");
	fseek(file, 0L, SEEK_END);
	unsigned int size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	unsigned char* data = new unsigned char[size];
	fread(data, 1, size, file);
	fclose(file);
	void* result = func(data, size, param);
	if(doneCallback)
		doneCallback(result);
}