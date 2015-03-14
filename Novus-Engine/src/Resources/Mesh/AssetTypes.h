//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once


#ifndef NOVUS_ASSET_TYPES_H
#define NOVUS_ASSET_TYPES_H

#include "Application/Common.h"

namespace novus
{
	namespace assettypes
	{
		namespace TextureType
		{
			enum Type
			{
				Unknown = -1,
				None,
				Diffuse,
				Specular,
				Emissive,
				Height,
				Normal,
				Roughness,
				Shininess,
				Lightmap,
				Mask,
			};
		}

		struct Bone
		{
			unsigned mId;
			Matrix4 mOffset;
		};

		struct VertexWeight
		{
			unsigned mVertexId;
		};

		struct BoneVertexWeights
		{
			//Unsigned is bone ID, float is weight
			std::vector<std::pair<unsigned, float>> boneWeights;
		};

		struct Material
		{
			Vector3 ambient;
			Vector3 emissive;
			Vector4 diffuse;
			Vector3 specular;
			float specularPow;

			std::string name;
			std::map<TextureType::Type, std::wstring> texturePaths;
		};

		struct Face
		{
			unsigned int mIndexCount;
			unsigned int mIndices[4];

			Face()
				:mIndexCount(0)
			{
				mIndices[0] = 0;
				mIndices[1] = 0;
				mIndices[2] = 0;
				mIndices[3] = 0;
			}

			~Face()
			{
			}

			Face(const Face& other)
			{
				*this = other;
			}

			Face& operator= (const Face& other)
			{
				if (&other == this)
					return *this;

				mIndexCount = other.mIndexCount;

				memcpy(mIndices, other.mIndices, mIndexCount * sizeof(unsigned int));

				return *this;
			}

			bool operator== (const Face& other)
			{
				if (mIndices && mIndexCount == other.mIndexCount)
				{
					for (unsigned int i = 0; i < this->mIndexCount; ++i)
					{
						if (mIndices[i] != other.mIndices[i])
							return false;
					}
					return true;
				}
				return false;
			}

			bool operator!= (const Face& other)
			{
				return !(*this == other);
			}
		};

		struct Mesh
		{
			std::string mMeshId;

			int mMaterialId;

			unsigned mVertexCount;
			unsigned mFaceCount;

			Vector3* mVertices;

			Vector3* mNormals;

			Vector3* mTangents;

			Vector2* mTextureCoords;

			Face* mFaces;

			Mesh()
				:mVertexCount(0),
				mFaceCount(0),
				mMaterialId(-1),
				mVertices(NULL),
				mNormals(NULL),
				mTangents(NULL),
				mTextureCoords(NULL),
				mFaces(NULL)
			{
			}

			~Mesh()
			{
				NE_DELETEARR(mVertices);
				NE_DELETEARR(mNormals);
				NE_DELETEARR(mTangents);
				NE_DELETEARR(mTextureCoords);
				delete [] mFaces; //Tracking does not work correctly on Faces
			}

			bool hasPositions() const { return mVertices != NULL && mVertexCount > 0; }
			bool hasFaces() const { return mFaces != NULL && mFaceCount > 0; }
			bool hasNormals() const { return mNormals != NULL && mVertexCount > 0; }
			bool hasTangents() const { return mTangents != NULL && mVertexCount > 0; }
			bool hasTextureCoords() const { return mTextureCoords != NULL && mVertexCount > 0; }
		};

		struct Node
		{
			std::string mName;

			Node* mpParent;

			Node** mpChildren;
			unsigned mNumChildren;

			Matrix4 mTransform;

			unsigned mNumMeshes;

			//Array of indices to meshes
			unsigned* mpMeshes;

			Node()
				:mpParent(NULL),
				mpChildren(NULL),
				mNumChildren(0),
				mNumMeshes(0),
				mpMeshes(NULL)
			{}

			~Node()
			{
				if (mpChildren != NULL)
				{
					for (unsigned int i = 0; i < mNumChildren; i++)
					{
						NE_DELETE(mpChildren[i]);
					}

					NE_DELETEARR(mpChildren);
					NE_DELETEARR(mpMeshes);
				}
			}
		};

		struct DirectionalLight
		{
			Vector3 direction;
			Vector3 diffuse;
			Vector3 ambient;
		};

		struct PointLight
		{
			Vector3 position;
			Vector3 diffuse;
			Vector3 ambient;
			float falloffLinear, falloffSquare;
			float range;
		};

		struct SpotLight
		{

		};

		struct Scene
		{
			Node* mpRootNode;

			std::vector<Mesh*> mMeshes;
			std::vector<Material*> mMaterials;

			Scene()
				:mpRootNode(NULL)
			{
			}

			~Scene()
			{
				NE_DELETE(mpRootNode);

				for (unsigned int i = 0; i < mMeshes.size(); i++)
					NE_DELETE(mMeshes[i]);

				for (unsigned int i = 0; i < mMaterials.size(); i++)
					NE_DELETE(mMaterials[i]);
			}
		};
	} //namespace assettypes
} //namespace novus

#endif