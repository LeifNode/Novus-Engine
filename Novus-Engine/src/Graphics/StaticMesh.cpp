#include "StaticMesh.h"
#include "Application/EngineStatics.h"
#include "Utils/StringUtils.h"
#include "Textures/Texture2D.h"
#include "Resources/ResourceCache.h"

namespace novus
{

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

	for (auto it = mMeshMaterials.begin(); it != mMeshMaterials.end(); ++it)
	{
		NE_DELETE(it->second);
	}
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
				}
			}

			MeshRenderer<StaticMeshVertex>* newMesh = AddMesh(vertices, indices);

			//Parse the mesh material
			if ((*it)->mMaterialId != -1)
			{
				StaticMeshMaterial* mat = NE_NEW StaticMeshMaterial();

				mat->RenderMaterial.Diffuse = meshes->mMaterials[(*it)->mMaterialId]->diffuse;
				mat->RenderMaterial.SpecularColor = meshes->mMaterials[(*it)->mMaterialId]->specular;
				mat->RenderMaterial.Emissive = meshes->mMaterials[(*it)->mMaterialId]->emissive;
				mat->RenderMaterial.Roughness = 1.0f - meshes->mMaterials[(*it)->mMaterialId]->specularPow * 0.01f;
				mat->RenderMaterial.HasDiffuseTexture = false;
				mat->RenderMaterial.HasNormalTexture = false;
				mat->RenderMaterial.HasSpecularTexture = false;

				mMeshMaterials[newMesh] = mat;

				auto endIt = meshes->mMaterials[(*it)->mMaterialId]->texturePaths.cend();
				for (auto texIt = meshes->mMaterials[(*it)->mMaterialId]->texturePaths.cbegin(); texIt != endIt; ++texIt)
				{
					switch (texIt->first)
					{
					case assettypes::TextureType::Diffuse:
						mat->Textures.push_back(std::pair<assettypes::TextureType::Type, Texture2D*>(assettypes::TextureType::Diffuse, EngineStatics::getResourceCache()->getResource<Texture2D>(texIt->second)));
						mat->RenderMaterial.HasDiffuseTexture = true;
						break;
					case assettypes::TextureType::Normal:
						mat->Textures.push_back(std::pair<assettypes::TextureType::Type, Texture2D*>(assettypes::TextureType::Normal, EngineStatics::getResourceCache()->getResource<Texture2D>(texIt->second)));
						mat->RenderMaterial.HasNormalTexture = true;
						break;
					case assettypes::TextureType::Specular:
						mat->Textures.push_back(std::pair<assettypes::TextureType::Type, Texture2D*>(assettypes::TextureType::Specular, EngineStatics::getResourceCache()->getResource<Texture2D>(texIt->second)));
						mat->RenderMaterial.HasSpecularTexture = true;
						break;
					}
				}
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
		perObject.World = renderer->GetTopTransform();
		perObject.WorldView = perObject.World * perFrame->View;
		perObject.WorldViewProj = perObject.World * perFrame->ViewProj;
		perObject.WorldInvTranspose = Matrix4::Inverse(Matrix4::Transpose(perObject.World));
		perObject.TextureTransform = Matrix4(1.0f);

		auto material = mMeshMaterials.find(*it);
		if (material != mMeshMaterials.end())
		{
			perObject.Material = material->second->RenderMaterial;

			for (auto texIt = material->second->Textures.cbegin(); texIt != material->second->Textures.cend(); ++texIt)
			{
				switch (texIt->first)
				{
				case assettypes::TextureType::Diffuse:
					renderer->setTextureResource(0, texIt->second);
					break;
				case assettypes::TextureType::Normal:
					renderer->setTextureResource(1, texIt->second);
					break;
				case assettypes::TextureType::Specular:
					renderer->setTextureResource(2, texIt->second);
					break;
				}
			}
		}

		perObject.Material.Roughness = 0.1f;

		renderer->setPerObjectBuffer(perObject);

		(*it)->Render(renderer);
	}
}

}
