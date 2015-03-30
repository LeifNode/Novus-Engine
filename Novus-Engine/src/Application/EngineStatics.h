//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_STATICS_H
#define NOVUS_STATICS_H

#include "Common.h"

namespace novus
{

class D3DRenderer;
class NovusApplication;
class InputSystem;
class EventSystem;
class FontManager;
class FileSystem;
class ResourceCache;
class World;

class EngineStatics
{
	friend class NovusApplication;

public:
	static NovusApplication* getApplication();
	static D3DRenderer* getRenderer();
	static InputSystem* getInputSystem();
	static EventSystem* getEventSystem();
	static FontManager* getFontManager();
	static FileSystem* getFileSystem();
	static ResourceCache* getResourceCache();
	static World* getWorld();

private:
	static NovusApplication* mspApplication;
	static D3DRenderer* mspRenderer;
	static InputSystem* mspInputSystem;
	static EventSystem* mspEventSystem;
	static FontManager* mspFontManager;
	static FileSystem* mspFileSystem;
	static ResourceCache* mspResourceCache;
	static World* mspWorld;
};

}
#endif