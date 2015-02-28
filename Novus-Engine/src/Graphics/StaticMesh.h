//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_STATIC_MESH
#define NOVUS_STATIC_MESH

#include "Math/Math.h"
#include "MeshRenderer.h"
#include "IRenderable.h"
#include "Shaders/Shader.h"
#include "Resources/Model/AssetTypes.h"

namespace novus
{
struct StaticMeshVertex
{
	Vector3 Position;
	Vector3 Normal;
	Vector3 Tangent;
	Vector2 TexCoord;
};

//TODO: Move storage into a managed resource class
// so that I don't have to reload the resource every time I query it
class StaticMesh : public IRenderable
{
public:
	StaticMesh();
	virtual ~StaticMesh();

	void Init(novus::assettypes::Scene* meshes);
	void AddMesh(std::vector<StaticMeshVertex>& vertices, std::vector<unsigned int>& indices);

	void Render(D3DRenderer* renderer) override;

private:
	StaticMesh(const StaticMesh& other) {}
	StaticMesh& operator= (const StaticMesh& other) { return *this; }

private:
	std::vector<MeshRenderer<StaticMeshVertex>*> mMeshes;
};
}

#endif