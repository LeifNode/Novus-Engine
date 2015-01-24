//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_CONSTANT_BUFFERs_H
#define NOVUS_CONSTANT_BUFFERS_H

#include "Application/Common.h"

namespace novus
{

struct CBPerFrame
{
	XMMATRIX View;
	XMMATRIX ViewInv;
	XMMATRIX Projection;
	XMMATRIX ProjectionInv;

	XMMATRIX ViewProj;
	XMMATRIX ViewProjInv;

	XMFLOAT3 HeadPosition;
	float pad;
	XMFLOAT3 EyePosition;
	float pad2;
	XMFLOAT3 EyeDirection;
	float pad3;

	//DirectionalLight DirectionalLight;
	//PointLight PointLight;
	//SpotLight SpotLight;
};

struct CBPerObject
{
	XMMATRIX World;
	XMMATRIX WorldViewProj;
	XMMATRIX WorldInvTranspose;
	XMMATRIX TextureTransform;
	//Material Material;
};

}

#endif