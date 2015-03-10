//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_MESH_TRIANGULATE_PASS_H
#define NOVUS_MESH_TRIANGULATE_PASS_H

#include "IMeshProcess.h"
#include "Resources/Mesh/AssetTypes.h"

namespace novus
{

class MeshTriangulatePass : public IMeshProcess
{
public:
	MeshTriangulatePass();
	~MeshTriangulatePass();

	virtual void Execute(assettypes::Mesh* mesh) override;
};

}

#endif