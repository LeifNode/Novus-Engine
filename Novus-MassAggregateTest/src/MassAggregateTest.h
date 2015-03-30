//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef MASS_AGGREGATE_TEST_H
#define MASS_AGGREGATE_TEST_H

#define DEBUG_MEMORY_ALLOC

#include <Application/Common.h>
#include <Application/NovusApplication.h>
#include <Graphics/Shaders/Shader.h>
#include <Graphics/ConstantBuffers.h>
#include <Graphics/MeshRenderer.h>
#include <Graphics/Geometry.h>
#include <Events/EventSystem.h>
#include <Math/Math.h>

namespace novus
{
	class Camera;
	class Texture2D;
	class Texture3D;
	class SkyboxRenderer;
	class StaticMesh;
	class PhysicsSystem;
}

class MassAggregateTest : public novus::NovusApplication
{
public:
	MassAggregateTest(HINSTANCE instance);
	virtual ~MassAggregateTest();

	virtual bool Init() override;
	virtual void OnResize() override;
	virtual void Update(float dt) override;
	virtual void Render() override;

	void HookInputEvents();
	void UnhookInputEvents();

	void OnKeyDown(novus::IEventDataPtr eventData);

	void InitShaders();
	void InitMesh();

private:
	novus::SkyboxRenderer* mpSkyboxRenderer;

	novus::PhysicsSystem* mpPhysicsSystem;

	novus::Camera* mpCamera;

	novus::Shader* mpMainShader;

	novus::CBPerFrame mPerFrameData;

	novus::MeshRenderer<novus::Vertex> mMeshRenderer;
	novus::MeshRenderer<novus::Vertex> mPlaneRenderer;
};


#endif