#include "EngineStatics.h"

using novus::EngineStatics;
using novus::D3DRenderer;
using novus::NovusApplication;
using novus::InputSystem;
using novus::EventSystem;
using novus::FontManager;

D3DRenderer* EngineStatics::mspRenderer = NULL;
NovusApplication* EngineStatics::mspApplication = NULL;
InputSystem* EngineStatics::mspInputSystem = NULL;
EventSystem* EngineStatics::mspEventSystem = NULL;
FontManager* EngineStatics::mspFontManager = NULL;

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

FontManager* EngineStatics::getFontManager()
{
	return mspFontManager;
}