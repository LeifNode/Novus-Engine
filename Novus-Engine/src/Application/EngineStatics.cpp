#include "EngineStatics.h"

namespace novus
{

D3DRenderer* EngineStatics::mspRenderer = NULL;
NovusApplication* EngineStatics::mspApplication = NULL;
InputSystem* EngineStatics::mspInputSystem = NULL;
EventSystem* EngineStatics::mspEventSystem = NULL;
FontManager* EngineStatics::mspFontManager = NULL;
FileSystem* EngineStatics::mspFileSystem = NULL;
ResourceCache* EngineStatics::mspResourceCache = NULL;

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

FileSystem* EngineStatics::getFileSystem()
{
	return mspFileSystem;
}

ResourceCache* EngineStatics::getResourceCache()
{
	return mspResourceCache;
}

}//namespace novus