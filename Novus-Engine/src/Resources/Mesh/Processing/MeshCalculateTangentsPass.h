//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_CALCULATE_TANGENTS_PASS_H
#define NOVUS_CALCULATE_TANGENTS_PASS_H

#include "IMeshProcess.h"
#include "Resources/Mesh/AssetTypes.h"

namespace novus
{

	class MeshCalculateTangentsPass : public IMeshProcess
	{
	public:
		MeshCalculateTangentsPass();
		~MeshCalculateTangentsPass();

		virtual void Execute(assettypes::Mesh* mesh) override;
	};

}

#endif

