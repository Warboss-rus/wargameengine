#pragma once
#ifdef DIRECTX
#include <Windows.h>
#define KEY_BACKSPACE VK_BACK
#define KEY_LEFT VK_LEFT
#define KEY_UP VK_UP
#define KEY_RIGHT VK_RIGHT
#define KEY_DOWN VK_DOWN
#define KEY_HOME VK_HOME
#define KEY_END VK_END
#define KEY_DELETE VK_DELETE
#elif GLFW
#include <GLFW\glfw3.h>
#define KEY_BACKSPACE GLFW_KEY_BACKSPACE
#define KEY_LEFT GLFW_KEY_LEFT
#define KEY_UP GLFW_KEY_UP
#define KEY_RIGHT GLFW_KEY_RIGHT
#define KEY_DOWN GLFW_KEY_DOWN
#define KEY_HOME GLFW_KEY_HOME
#define KEY_END GLFW_KEY_END
#define KEY_DELETE GLFW_KEY_DELETE
#else
#include <GL/freeglut_std.h>

#define KEY_BACKSPACE 8
#define KEY_LEFT GLUT_KEY_LEFT
#define KEY_UP GLUT_KEY_UP
#define KEY_RIGHT GLUT_KEY_RIGHT
#define KEY_DOWN GLUT_KEY_DOWN
#define KEY_HOME GLUT_KEY_HOME
#define KEY_END GLUT_KEY_END
#define KEY_DELETE 127
#endif