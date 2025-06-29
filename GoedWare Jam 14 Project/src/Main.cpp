#include "Engine.h"
#include <Windows.h>

#ifdef NDEBUG
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int main()
#endif
{
	Engine::Instance()->RunEngine();

	return 0;
}