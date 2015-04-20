//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef TEST_APPLICATION_H
#define TEST_APPLICATION_H

#define DEBUG_MEMORY_ALLOC

#include <Application/Common.h>
#include <Application/NovusApplication.h>
#include <Graphics/Shaders/Shader.h>
#include <Graphics/ConstantBuffers.h>
#include <Graphics/MeshRenderer.h>
#include <Graphics/Geometry.h>
#include <Events/EventSystem.h>
#include <Math/Math.h>
#include <Graphics/Lights.h>

namespace novus
{
	class Actor;
	class Camera;
	class Texture2D;
	class Texture3D;
	class SkyboxRenderer;
	class StaticMesh;
	class ShadowMapRenderTarget;
	class VoxelVolumeRenderTarget;
	class GlobalIlluminationPass;
	class VoxelRadianceVolume;
}

class VXGITestApplication : public novus::NovusApplication
{
public:
	VXGITestApplication(HINSTANCE instance);
	virtual ~VXGITestApplication();

	virtual bool Init() override;
	virtual void OnResize() override;
	virtual void Update(float dt) override;
	virtual void Render() override;

	void HookInputEvents();
	void UnhookInputEvents();

	void OnKeyDown(novus::IEventDataPtr eventData);

	void InitShaders();
	void InitLights();
	void InitBoxes();

private:
	novus::SkyboxRenderer* mpSkyboxRenderer;

	novus::Camera* mpCamera;

	novus::Shader* mpStaticMeshShader;
	novus::Shader* mpVoxelizationShader;
	novus::Shader* mpDebugRaymarchShader;
	novus::Shader* mpDepthPassShader;

	novus::ShadowMapRenderTarget* mpShadowMap;
	novus::VoxelVolumeRenderTarget* mpVoxelVolume;
	novus::GlobalIlluminationPass* mpGlobalIlluminationRenderPass;

	novus::VoxelRadianceVolume* mpRadianceVolume;

	novus::CBPerFrame mPerFrameData;

	novus::Actor* mpSphereActor;

	novus::StaticMesh* mpMesh;
	novus::StaticMesh* mpSphereMesh;

	std::vector<novus::Actor*> mObjects;
	std::vector<novus::StaticMesh*> mObjectMeshes;

	novus::Texture3D* mpVoxelTexture;

	std::vector<novus::PointLight> mLights;

	float mShadowOffsetX;
	float mShadowOffsetY;
	bool mRequiresReinject;

	bool mSceneVoxelized;
	bool mRenderVoxelization;
	int mVoxelResolution;
};


#endif