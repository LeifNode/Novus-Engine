#include "StaticMesh.h"
#include "Application/EngineStatics.h"

namespace novus
{

StaticMesh::StaticMesh()
{
}

StaticMesh::~StaticMesh()
{
	for (auto it = mMeshes.begin(); it != mMeshes.end(); ++it)
	{
		NE_DELETE((*it));
	}
}

void StaticMesh::Init(assettypes::Scene* meshes)
{
	std::vector<StaticMeshVertex> vertices;
	std::vector<unsigned int> indices;

	for (auto it = meshes->mMeshes.cbegin(); it != meshes->mMeshes.cend(); ++it)
	{
		vertices.clear();
		indices.clear();

		vertices.reserve((*it)->mVertexCount);
		
		for (unsigned int i = 0; i < (*it)->mVertexCount; i++)
		{
			StaticMeshVertex vertex;

			vertex.Position = (*it)->mVertices[i];

			if ((*it)->hasNormals())
				vertex.Normal = (*it)->mNormals[i];
			else
				vertex.Normal = Vector3(0.0f);

			if ((*it)->hasTextureCoords())
				vertex.TexCoord = (*it)->mTextureCoords[i];
			else
				vertex.TexCoord = Vector2(0.0f);

			if ((*it)->mTangents != NULL)
				vertex.Tangent = (*it)->mTangents[i];
			else
				vertex.Tangent = Vector3(0.0f);

			vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < (*it)->mFaceCount; i++)
		{
			if ((*it)->mFaces[i].mIndexCount == 3)
			{
				indices.push_back((*it)->mFaces[i].mIndices[0]);
				indices.push_back((*it)->mFaces[i].mIndices[1]);
				indices.push_back((*it)->mFaces[i].mIndices[2]);
			}
			else if ((*it)->mFaces[i].mIndexCount == 4)
			{
				indices.push_back((*it)->mFaces[i].mIndices[0]);
				indices.push_back((*it)->mFaces[i].mIndices[1]);
				indices.push_back((*it)->mFaces[i].mIndices[2]);

				indices.push_back((*it)->mFaces[i].mIndices[3]);
				indices.push_back((*it)->mFaces[i].mIndices[0]);
				indices.push_back((*it)->mFaces[i].mIndices[2]);
			}
		}

		AddMesh(vertices, indices);
	}
}

void StaticMesh::AddMesh(std::vector<StaticMeshVertex>& vertices, std::vector<unsigned int>& indices)
{
	MeshRenderer<novus::StaticMeshVertex>* newMesh = NE_NEW MeshRenderer<novus::StaticMeshVertex>();

	newMesh->Init(EngineStatics::getRenderer(), vertices, indices);

	mMeshes.push_back(newMesh);
}

void StaticMesh::Render(D3DRenderer* renderer)
{
	for (auto it = mMeshes.cbegin(); it != mMeshes.cend(); ++it)
	{
		(*it)->Render(renderer);
	}
}

}
