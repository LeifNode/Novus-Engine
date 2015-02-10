//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

//Adapted from code accompanying the book Introduction to 3D Game Programming with DirectX 11 code written by Frank Luna

#ifndef NOVUS_GEOMETRY_H
#define NOVUS_GEOMETRY_H


#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <list>
#include "Application/Common.h"
#include "Math/Math.h"

namespace novus
{

struct Vertex
{
	Vector3 Position;
	Vector3 Normal;
	Vector3 Tangent;
	Vector3 Bitangent;
	Vector2 TexCoord;

	Vertex(){ ZeroMemory(this, sizeof(this)); }

	Vertex(const Vector3& position, const Vector3& normal)
		:Position(position),
		Normal(normal)
	{}

	Vertex(const Vector3& position, const Vector3& normal, const Vector2& texCoord)
		:Position(position),
		Normal(normal),
		TexCoord(texCoord)
	{}

	Vertex(const Vector3& position, const Vector3& normal, const Vector2& texCoord, const Vector3& tangent, const Vector3& bitangent)
		:Position(position),
		Normal(normal),
		TexCoord(texCoord),
		Tangent(tangent),
		Bitangent(bitangent)
	{}

	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float bx, float by, float bz,
		float u, float v)
		: Position(px, py, pz), Normal(nx, ny, nz),
		Tangent(tx, ty, tz), Bitangent(bx, by, bz), TexCoord(u, v)
	{}
};

struct SimpleVertex
{
	SimpleVertex(){}
	SimpleVertex(const Vector3& p, const Vector3& n)
		:Position(p), Normal(n){}
	SimpleVertex(
		float px, float py, float pz,
		float nx, float ny, float nz)
		: Position(px, py, pz), Normal(nx, ny, nz){}

	Vector3 Position;
	Vector3 Normal;
};

struct Mesh
{
	std::vector<Vertex> Vertices;
	std::vector<UINT> Indices;
};

struct SimpleMesh
{
	std::vector<SimpleVertex> Vertices;
	std::vector<UINT> Indices;
};

static const D3D11_INPUT_ELEMENT_DESC SimpleVertexDescription[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

static const D3D11_INPUT_ELEMENT_DESC VertexDescription[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

class GeometryGenerator
{
public:


public:
	///<summary>
	/// Creates a box centered at the origin with the given dimensions.
	///</summary>
	static void CreateBox(float width, float height, float depth, Mesh& mesh);

	static void CreateBox(float width, float height, float depth, SimpleMesh& mesh);

	///<summary>
	/// Creates a sphere centered at the origin with the given radius.  The
	/// slices and stacks parameters control the degree of tessellation.
	///</summary>
	static void CreateSphere(float radius, UINT sliceCount, UINT stackCount, Mesh& mesh);

	///<summary>
	/// Creates a geosphere centered at the origin with the given radius.  The
	/// depth controls the level of tessellation.
	///</summary>
	static void CreateGeosphere(float radius, UINT numSubdivisions, Mesh& mesh);

	///<summary>
	/// Creates a cylinder parallel to the y-axis, and centered about the origin.  
	/// The bottom and top radius can vary to form various cone shapes rather than true
	// cylinders.  The slices and stacks parameters control the degree of tessellation.
	///</summary>
	static void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, Mesh& mesh);

	///<summary>
	/// Creates an mxn grid in the xz-plane with m rows and n columns, centered
	/// at the origin with the specified width and depth.
	///</summary>
	static void CreateGrid(float width, float depth, UINT m, UINT n, Mesh& mesh);

	///<summary>
	/// Creates a quad covering the screen in NDC coordinates.  This is useful for
	/// postprocessing effects.
	///</summary>
	static void CreateFullscreenQuad(Mesh& mesh);

	static void CreatePlayerArrow(Mesh& mesh);

	static void CreateSkybox(std::vector<Vector3>& vertices, std::vector<UINT>& indices);

private:
	static void Subdivide(Mesh& meshData);
	static void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, Mesh& mesh);
	static void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, Mesh& mesh);
};

}
#endif