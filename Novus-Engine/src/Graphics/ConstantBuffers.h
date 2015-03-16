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

struct Material
{
	Vector4 Diffuse;
	Vector3 SpecularColor;
	float Roughness;
	Vector3 Emissive;
	float Metallic;

	int HasDiffuseTexture;
	int HasNormalTexture;
	int HasSpecularTexture; 
	int pad3;
};

struct CBPerFrame
{
	Matrix4 View;
	Matrix4 ViewInv;
	Matrix4 Projection;
	Matrix4 ProjectionInv;

	Matrix4 ViewProj;
	Matrix4 ViewProjInv;

	Vector2u ScreenResolution;
	Vector2u pad4;
	Vector2 ClipNearFar;
	Vector2 pad5;

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
	Material Material;
};

}

#endif