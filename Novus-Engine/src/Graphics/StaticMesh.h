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
#include "Resources/Mesh/AssetTypes.h"
#include "Resources/Resource.h"
#include "Utils/Objects/Type.h"
#include "Geometry.h"

namespace novus
{
struct Vertex;

//struct StaticMeshVertex
//{
//	Vector3 Position;
//	Vector3 Normal;
//	Vector3 Tangent;
//	Vector2 TexCoord;
//};

struct StaticMeshMaterial
{
	Material RenderMaterial;

	std::vector<std::pair<assettypes::TextureType::Type, class Texture2D*>> Textures;
};

//TODO: Move storage into a managed resource class
// so that I don't have to reload the resource every time I query it
class StaticMesh : public IRenderable, public Resource
{
	NOVUS_OBJECT_DECL(StaticMesh);

public:
	StaticMesh();
	virtual ~StaticMesh();

	void Init(novus::assettypes::Scene* meshes);
	MeshRenderer<Vertex>* AddMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

	void Render(D3DRenderer* renderer) override;

	size_t getDiskSize() const override { return 0; }
	size_t getGPUSize() const override { return 0; }
	size_t getSize() const override { return 0; }

	void setMaterial(const StaticMeshMaterial& material);

	void Free();

private:
	StaticMesh(const StaticMesh& other) {}
	StaticMesh& operator= (const StaticMesh& other) { return *this; }

private:
	StaticMeshMaterial mMaterialFallback;
	bool mFallbackSet;

	std::vector<MeshRenderer<Vertex>*> mMeshes;
	std::map<MeshRenderer<Vertex>*, StaticMeshMaterial*> mMeshMaterials;
};

NOVUS_OBJECT_DEF(StaticMesh);

}

#endif