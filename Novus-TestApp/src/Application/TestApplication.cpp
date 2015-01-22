#include "TestApplication.h"

#include <Graphics/D3DRenderer.h>

using novus::NovusApplication;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd){
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	TestApplication* application = NE_NEW TestApplication(hInstance);

	if (!application->Init())
		return 0;

	int returnVal = application->Run();

	NE_DELETE(application);

	return returnVal;
}

TestApplication::TestApplication(HINSTANCE instance)
	:
	NovusApplication(instance),
	mpMainShader(NULL)
{
	mMainWndCaption = L"Novus Engine Test App v0.0.1";
}

TestApplication::~TestApplication()
{
	NE_DELETE(mpMainShader);
}

void TestApplication::Cleanup()
{
	
}

bool TestApplication::Init()
{
	if (!NovusApplication::Init())
		return false;

	OnResize();

	return true;
}

void TestApplication::OnResize()
{

}

void TestApplication::Update(float dt)
{
}

void TestApplication::Render()
{
	mpRenderer->PreRender();


	mpRenderer->PostRender();
}