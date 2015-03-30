#include "LightManager.h"

namespace novus
{

LightManager::LightManager()
{

}

LightManager::~LightManager()
{

}

void LightManager::AddLightForFrame(const PointLight& light)
{

}

void LightManager::ClearLights()
{

}

const std::vector<PointLight>& LightManager::getLights()
{
	return mLights;
}

}