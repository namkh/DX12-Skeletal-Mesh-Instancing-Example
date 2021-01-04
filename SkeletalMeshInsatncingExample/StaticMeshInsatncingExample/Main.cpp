
#define WIN32_LEAN_AND_MEAN 
#include "Win32Application.h"
#include "GlobalSystemValues.h"
#include "SkeletalMeshInstancingExample.h"

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	SkeletalMeshInstancingExample example(L"Mesh Shader Instancing", GlobalSystemValues::Instance().ScreenWidth, GlobalSystemValues::Instance().ScreenHeight);

	return Win32Application::Run(&example, hInstance, nCmdShow);
}