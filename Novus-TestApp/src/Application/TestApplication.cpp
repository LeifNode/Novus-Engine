#include "TestApplication.h"

#include <Application/EngineStatics.h>
#include <Graphics/D3DRenderer.h>
#include <Events/Events.h>
#include <Events/EventSystem.h>

using namespace novus;

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
	UnhookInputEvents();

	NE_DELETE(mpMainShader);
}

bool TestApplication::Init()
{
	if (!NovusApplication::Init())
		return false;

	OnResize();

	HookInputEvents();

	return true;
}

void TestApplication::HookInputEvents()
{
	EngineStatics::getEventSystem()->AddListener(EventData_KeyboardDown::skEventType, fastdelegate::MakeDelegate(this, &TestApplication::OnKeyDown));
}

void TestApplication::UnhookInputEvents()
{
	EngineStatics::getEventSystem()->RemoveListener(EventData_KeyboardDown::skEventType, fastdelegate::MakeDelegate(this, &TestApplication::OnKeyDown));
}

void TestApplication::OnKeyDown(novus::IEventDataPtr eventData)
{
	auto dataPtr = static_pointer_cast<EventData_KeyboardDown>(eventData);

	if (dataPtr->getKey() == KeyboardKey::KEY_ESC)
	{
		ExitApp();
	}
}

void TestApplication::OnResize()
{
	NovusApplication::OnResize();


}

void TestApplication::Update(float dt)
{



}

void TestApplication::Render()
{
	mpRenderer->PreRender();


	mpRenderer->PostRender();
}