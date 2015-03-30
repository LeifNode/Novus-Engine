//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_STATIC_MESH_COMPONENT_H
#define NOVUS_STATIC_MESH_COMPONENT_H

#include "Core/ActorComponent.h"
#include "Graphics/StaticMesh.h"

namespace novus
{

class StaticMeshComponent : public ActorComponent
{
	NOVUS_OBJECT_DECL(StaticMeshComponent);

public:
	StaticMeshComponent(StaticMesh* mesh);
	~StaticMeshComponent();

	void Render(D3DRenderer* renderer) override;

	void setStaticMesh(StaticMesh* mesh);

	StaticMesh* getStaticMesh() const;

private:
	StaticMesh* mpStaticMesh;
};

NOVUS_OBJECT_DEF(StaticMeshComponent);

}


#endif