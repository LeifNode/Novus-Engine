//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_MESH_PROCESS_H
#define NOVUS_MESH_PROCESS_H

#include "Resources/Model/AssetTypes.h"

namespace novus
{

class IMeshProcess
{
public:
	IMeshProcess() {};
	virtual ~IMeshProcess() {};

	virtual void Execute(assettypes::Mesh* mesh)=0;
};

}

#endif