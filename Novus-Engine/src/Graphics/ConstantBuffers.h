//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_CONSTANT_BUFFERS_H
#define NOVUS_CONSTANT_BUFFERS_H

#include "Application/Common.h"
#include "Math/Math.h"

namespace novus
{

struct CBPerFrame
{
	Matrix4 View;
	Matrix4 ViewInv;
	Matrix4 Projection;
	Matrix4 ProjectionInv;

	Matrix4 ViewProj;
	Matrix4 ViewProjInv;

	Vector2_t<unsigned int> ScreenResolution;
	Vector2_t<unsigned int> pad;

	Vector3 HeadPosition;
	float pad1;
	Vector3 EyePosition;
	float pad2;
	Vector3 EyeDirection;
	float pad3;

	//DirectionalLight DirectionalLight;
	//PointLight PointLight;
	//SpotLight SpotLight;
};

struct CBPerObject
{
	Matrix4 World;
	Matrix4 WorldViewProj;
	Matrix4 WorldInvTranspose;
	Matrix4 TextureTransform;
	//Material Material;
};

}

#endif