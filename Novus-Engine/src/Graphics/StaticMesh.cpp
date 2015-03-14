#include "StaticMesh.h"
#include "Application/EngineStatics.h"
#include "Utils/StringUtils.h"

namespace novus
{

Type StaticMesh::msType = Type("StaticMesh");

StaticMesh::StaticMesh()
{
}

StaticMesh::~StaticMesh()
{
	this->Free();
}

void StaticMesh::Free()
{
	for (auto it = mMeshes.begin(); it != mMeshes.end(); ++it)
	{
		NE_DELETE(*it);
	}

	mMeshes.clear();
}

const Type* StaticMesh::getStaticType() const
{
	return &msType;
}

void StaticMesh::Init(assettypes::Scene* meshes)
{
	std::vector<StaticMeshVertex> vertices;
	std::vector<unsigned int> indices;

	if (meshes != NULL)
	{
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

			bool indexWarnTriggered = false;

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
					if (!indexWarnTriggered)
					{
						NE_WARN("StaticMesh triangulation is depricated use MeshTriangulatePass during model loading instead", "StaticMesh");
						indexWarnTriggered = true;
					}
					
					indices.push_back((*it)->mFaces[i].mIndices[0]);
					indices.push_back((*it)->mFaces[i].mIndices[1]);
					indices.push_back((*it)->mFaces[i].mIndices[2]);

					indices.push_back((*it)->mFaces[i].mIndices[3]);
					indices.push_back((*it)->mFaces[i].mIndices[0]);
					indices.push_back((*it)->mFaces[i].mIndices[2]);
				}
			}

			MeshRenderer<StaticMeshVertex>* newMesh = AddMesh(vertices, indices);

			if ((*it)->mMaterialId != -1)
			{
				Material mat;

				mat.Diffuse = meshes->mMaterials[(*it)->mMaterialId]->diffuse;
				mat.SpecularColor = meshes->mMaterials[(*it)->mMaterialId]->specular;
				mat.Emissive = meshes->mMaterials[(*it)->mMaterialId]->emissive;
				mat.Roughness = 1.0f - meshes->mMaterials[(*it)->mMaterialId]->specularPow * 0.01f;
				mat.HasDiffuseTexture = false;

				mMeshMaterials[newMesh] = mat;
			}
		}
	}
}

MeshRenderer<StaticMeshVertex>* StaticMesh::AddMesh(std::vector<StaticMeshVertex>& vertices, std::vector<unsigned int>& indices)
{
	MeshRenderer<novus::StaticMeshVertex>* newMesh = NE_NEW MeshRenderer<novus::StaticMeshVertex>();

	newMesh->Init(EngineStatics::getRenderer(), vertices, indices);

	mMeshes.push_back(newMesh);

	return newMesh;
}

void StaticMesh::Render(D3DRenderer* renderer)
{
	for (auto it = mMeshes.cbegin(); it != mMeshes.cend(); ++it)
	{
		const CBPerFrame* perFrame = renderer->getPerFrameBuffer();

		CBPerObject perObject;
		perObject.World = Matrix4(1.0f);
		perObject.WorldViewProj = perFrame->ViewProj;
		perObject.WorldInvTranspose = Matrix4(1.0f);

		auto material = mMeshMaterials.find(*it);
		if (material != mMeshMaterials.end())
			perObject.Material = material->second;

		renderer->setPerObjectBuffer(perObject);

		(*it)->Render(renderer);
	}
}

}
