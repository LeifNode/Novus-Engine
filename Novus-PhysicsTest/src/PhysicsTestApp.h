//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef PHYSICS_TEST_APPLICATION_H
#define PHYSICS_TEST_APPLICATION_H

#define DEBUG_MEMORY_ALLOC

#include <Application/Common.h>
#include <Application/NovusApplication.h>
#include <Graphics/Shader.h>
#include <Graphics/ConstantBuffers.h>
#include <Graphics/MeshRenderer.h>
#include <Graphics/Geometry.h>
#include <Events/EventSystem.h>
#include <Math/Math.h>

namespace novus
{
	class Camera;
}

class PhysicsTestApplication : public novus::NovusApplication
{
public:
	PhysicsTestApplication(HINSTANCE instance);
	virtual ~PhysicsTestApplication();

	virtual bool Init() override;
	virtual void OnResize() override;
	virtual void Update(float dt) override;
	virtual void Render() override;

	void HookInputEvents();
	void UnhookInputEvents();

	void OnKeyDown(novus::IEventDataPtr eventData);

	void InitShader();
	void InitMesh();

private:
	novus::Quaternion mCurrentRotation;

	novus::Camera* mpCamera;

	novus::Shader* mpMainShader;
	novus::Shader* mpTiledDeferredShader;

	novus::CBPerFrame mPerFrameData;

	novus::MeshRenderer<novus::Vertex> mMeshRenderer;
	novus::MeshRenderer<novus::Vertex> mPlaneRenderer;
};


#endif