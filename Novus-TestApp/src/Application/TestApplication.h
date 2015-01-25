//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef TEST_APPLICATION_H
#define TEST_APPLICATION_H

#include <Application/Common.h>
#include <Application/NovusApplication.h>
#include <Graphics/Shader.h>
#include <Graphics/ConstantBuffers.h>
#include <Graphics/MeshRenderer.h>
#include <Graphics/Geometry.h>
#include <Events/EventSystem.h>
#include <Math/Vector3_t.h>

class TestApplication : public novus::NovusApplication
{
public:
	TestApplication(HINSTANCE instance);
	virtual ~TestApplication();

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
	novus::math::Vector3 mTestVec;

	novus::Shader* mpMainShader;

	novus::CBPerFrame mPerFrameData;

	novus::MeshRenderer<Vertex> mMeshRenderer;
};


#endif