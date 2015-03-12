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
			Vector4 ambient;
			Vector4 emissive;
			Vector4 diffuse;
			Vector4 specular;
			float specularPow;

			std::map<TextureType::Type, std::string> texturePaths;
		};

		struct Face
		{
			unsigned mIndexCount;
			unsigned mIndices[4];

			Face()
				:mIndexCount(0)
			{
			}

			~Face()
			{
				//NE_DELETEARR(mIndices);
			}

			Face(const Face& other)
			{
				*this = other;
			}

			Face& operator= (const Face& other)
			{
				/*if (&other == this)
				return *this;

				NE_DELETEARR(mIndices);
				mIndexCount = other.mIndexCount;

				if (mIndexCount)
				{
				mIndices = NE_NEW unsigned[mIndexCount];
				memcpy(mIndices, other.mIndices, mIndexCount * sizeof(unsigned));
				}
				else
				mIndices = NULL;

				return *this;*/

				if (&other == this)
					return *this;

				mIndexCount = other.mIndexCount;

				memcpy(mIndices, other.mIndices, mIndexCount * sizeof(unsigned int));

				return *this;
			}

			bool operator== (const Face& other)
			{
				if (mIndices == other.mIndices)return true;
				else if (mIndices && mIndexCount == other.mIndexCount)
				{
					for (unsigned int i = 0; i < this->mIndexCount; ++i)
					if (mIndices[i] != other.mIndices[i])
						return false;
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

			unsigned mMaterialId;

			unsigned mVertexCount;
			unsigned mFaceCount;

			Vector3* mVertices;

			Vector3* mNormals;

			Vector3* mTangents;

			Vector3* mBitengents;

			Vector2* mTextureCoords;

			Face* mFaces;

			Mesh()
				:mVertexCount(0),
				mFaceCount(0),
				mMaterialId(0),
				mVertices(NULL),
				mNormals(NULL),
				mTangents(NULL),
				mBitengents(NULL),
				mTextureCoords(NULL),
				mFaces(NULL)
			{
			}

			~Mesh()
			{
				NE_DELETEARR(mVertices);
				NE_DELETEARR(mNormals);
				NE_DELETEARR(mTangents);
				NE_DELETEARR(mBitengents);
				NE_DELETEARR(mTextureCoords);
				NE_DELETEARR(mFaces);
			}

			bool hasPositions() const { return mVertices != NULL && mVertexCount > 0; }
			bool hasFaces() const { return mFaces != NULL && mFaceCount > 0; }
			bool hasNormals() const { return mNormals != NULL && mVertexCount > 0; }
			bool hasTangentsAndBitangents() const { return mTangents != NULL && mBitengents != NULL && mVertexCount > 0; }
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

				for (int i = 0; i < mMeshes.size(); i++)
					NE_DELETE(mMeshes[i]);

				for (int i = 0; i < mMaterials.size(); i++)
					NE_DELETE(mMaterials[i]);
			}
		};
	} //namespace assettypes
} //namespace novus

#endif