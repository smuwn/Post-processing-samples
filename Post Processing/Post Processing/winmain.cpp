#include "Scene.h"

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine, int iShow )
{
	srand( ( unsigned int ) time( NULL ) );
	Scene::Create(
		hInstance,
#if DEBUG || _DEBUG
		false
#else
		true
#endif
		);
	Scene::GetSceneInstance()->Run();
	Scene::Destroy();
	return 0;
}