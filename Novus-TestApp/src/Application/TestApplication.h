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

class TestApplication : public novus::NovusApplication
{
public:
	TestApplication(HINSTANCE instance);
	virtual ~TestApplication();

	void Cleanup();

	virtual bool Init() override;
	virtual void OnResize() override;
	virtual void Update(float dt) override;
	virtual void Render() override;

private:
	novus::Shader* mpMainShader;

	novus::CBPerFrame mPerFrameData;
};


#endif