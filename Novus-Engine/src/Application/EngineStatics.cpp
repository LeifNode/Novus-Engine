#include "EngineStatics.h"

using novus::EngineStatics;
using novus::D3DRenderer;
using novus::NovusApplication;
using novus::InputSystem;
using novus::EventSystem;

D3DRenderer* EngineStatics::mspRenderer = NULL;
NovusApplication* EngineStatics::mspApplication = NULL;
InputSystem* EngineStatics::mspInputSystem = NULL;
EventSystem* EngineStatics::mspEventSystem = NULL;

NovusApplication* EngineStatics::getApplication()
{
	return mspApplication;
}

D3DRenderer* EngineStatics::getRenderer()
{
	return mspRenderer;
}

InputSystem* EngineStatics::getInputSystem()
{
	return mspInputSystem;
}

EventSystem* EngineStatics::getEventSystem()
{
	return mspEventSystem;
}