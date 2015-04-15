//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_VOXEL_GI_COMMON_H
#define NOVUS_VOXEL_GI_COMMON_H

#include "Math/Math.h"

namespace novus
{

struct CBVoxelGI
{
	Matrix4 WorldToVoxelVolume;

	Vector3i VoxelVolumeDim;
	int VoxelMipCount;

	Vector3 pad;
	float VoxelScale; //Size of one voxel in meters
};

}

#endif