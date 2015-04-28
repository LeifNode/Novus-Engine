//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef RIGID_BODY_TEST_H
#define RIGID_BODY_TEST_H

#define DEBUG_MEMORY_ALLOC

#include <Application/Common.h>
#include <Application/NovusApplication.h>
#include <Graphics/Shaders/Shader.h>
#include <Graphics/ConstantBuffers.h>
#include <Graphics/MeshRenderer.h>
#include <Graphics/Geometry.h>
#include <Events/EventSystem.h>
#include <Math/Math.h>
#include <Physics/RigidBodyPhysicsSystem.h>

namespace novus
{
	class Camera;
	class Texture2D;
	class Texture3D;
	class SkyboxRenderer;
	class StaticMesh;
	class LineRenderer;
}

class RigidBodyTest : public novus::NovusApplication
{
public:
	RigidBodyTest(HINSTANCE instance);
	virtual ~RigidBodyTest();

	virtual bool Init() override;
	virtual void OnResize() override;
	virtual void Update(float dt) override;
	virtual void Render() override;

	void HookInputEvents();
	void UnhookInputEvents();

	void OnKeyDown(novus::IEventDataPtr eventData);

	void InitShaders();
	void InitMesh();
	void InitPhysicsActors();
	void InitPhysicsBounds();

	void ResetPhysicsSimulation();

private:
	void UpdateContactRenderers();
	void UpdatePointLights(float dt);
	void UpdatePhysics(float dt);

	void UpdateBodies(float dt);
	void GenerateContacts();

private:
	novus::SkyboxRenderer* mpSkyboxRenderer;

	novus::LineRenderer* mpContactRenderer;

	novus::Camera* mpCamera;

	novus::Shader* mpMainShader;

	novus::CBPerFrame mPerFrameData;

	novus::MeshRenderer<novus::Vertex> mMeshRenderer;
	novus::MeshRenderer<novus::Vertex> mPlaneRenderer;

	std::vector<novus::RigidBody*> mPhysicsBodies;
	std::vector<novus::CollisionBox*> mCollisionBoxes;
	std::vector<novus::CollisionPlane*> mCollisionHalfSpaces;

	static const unsigned int msMaxContacts = 1024;

	novus::Contact mContacts[msMaxContacts];
	unsigned int mContactCount;
	novus::CollisionData mCollisionData;
	novus::ContactResolver mContactResolver;


};


#endif