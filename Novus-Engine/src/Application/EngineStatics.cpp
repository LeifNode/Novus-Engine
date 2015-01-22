#include "EngineStatics.h"

using novus::EngineStatics;
using novus::D3DRenderer;
using novus::NovusApplication;

novus::D3DRenderer* EngineStatics::mspRenderer = NULL;
novus::NovusApplication* EngineStatics::mspApplication = NULL;

NovusApplication* EngineStatics::getApplication()
{
	return mspApplication;
}

D3DRenderer* EngineStatics::getRenderer()
{
	return mspRenderer;
}