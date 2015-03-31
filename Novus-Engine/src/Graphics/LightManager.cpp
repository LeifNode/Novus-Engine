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
	mLights.push_back(light);
}

void LightManager::ClearLights()
{
	mLights.clear();
}

const std::vector<PointLight>& LightManager::getLights()
{
	return mLights;
}

}