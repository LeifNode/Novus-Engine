//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once


#ifndef NOVUS_OBJ_LOADER_H
#define NOVUS_OBJ_LOADER_H

#include "MeshLoader.h"
#include "Utils/FileSystem/File.h"

namespace novus
{

class OBJLoader : public MeshLoader
{
	struct OBJFace
	{
		Vector3i Vertices[4];
		unsigned char VertexCount;
		bool HasNormals;
		bool HasTexCoords;
	};

public:
	OBJLoader();

	~OBJLoader() override;

	bool Load(File* file) override;

private:
	OBJLoader(const OBJLoader& other) {}
	OBJLoader& operator= (const OBJLoader& other) { return *this; }

	inline Vector2 ParseVector2(const char* str);
	inline Vector3 ParseVector3(const char* str);
	OBJFace ParseFace(const char* str);
	void StoreMesh(const std::vector<Vector3>& pos,
				   const std::vector<Vector2>& tex,
				   const std::vector<Vector3>& norm,
				   const std::vector<OBJFace>& faces,
				   unsigned int vertexCount,
				   assettypes::Mesh* meshOut);

	void LoadMatLib(const std::wstring& path);
	//assettypes::Material ParseMaterial(const char* str);

private:
	std::istream* mpFile;
};

}

#endif
