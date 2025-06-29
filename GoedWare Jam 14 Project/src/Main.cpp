#include "Engine.h"
#include <Windows.h>

int main()
{
	FreeConsole();

	Engine::Instance()->RunEngine();

	return 0;
}