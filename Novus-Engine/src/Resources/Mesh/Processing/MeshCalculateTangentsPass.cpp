#include "MeshCalculateTangentsPass.h"
#include "Math/Math.h"

namespace novus
{

MeshCalculateTangentsPass::MeshCalculateTangentsPass()
{}

MeshCalculateTangentsPass::~MeshCalculateTangentsPass()
{}

void MeshCalculateTangentsPass::Execute(assettypes::Mesh* mesh)
{
	if (!mesh->hasTextureCoords())
		return;

	NE_DELETEARR(mesh->mTangents);
	mesh->mTangents = NE_NEW Vector3[mesh->mVertexCount];

	Vector3 tangent;
	//Vector3 binormal;
	Vector3 uVector;
	Vector3 vVector;

	Vector3 pos1;
	Vector3 pos2;
	Vector3 pos3;

	Vector2 tex1;
	Vector2 tex2;
	Vector2 tex3;

	Vector3 tangentVector1, tangentVector2;

	for (unsigned int i = 0; i < mesh->mFaceCount; i++)
	{
		pos1 = mesh->mVertices[mesh->mFaces[i].mIndices[0]]; 
		pos2 = mesh->mVertices[mesh->mFaces[i].mIndices[1]];
		pos3 = mesh->mVertices[mesh->mFaces[i].mIndices[2]];

		tex1 = mesh->mTextureCoords[mesh->mFaces[i].mIndices[0]];
		tex2 = mesh->mTextureCoords[mesh->mFaces[i].mIndices[1]];
		tex3 = mesh->mTextureCoords[mesh->mFaces[i].mIndices[2]];

		tangentVector1 = pos2 - pos1;
		tangentVector2 = pos3 - pos1;

		uVector.x = tex2.x - tex1.x;
		uVector.y = tex3.x - tex1.x;

		vVector.x = tex2.y - tex1.y;
		vVector.y = tex3.y - tex1.y;

		float denominator = (uVector[0] * vVector[1] - uVector[1] * vVector[0]);

		if (denominator != 0.0f)
		{
			denominator = 1.0f / denominator;

			tangent.x = (vVector[1] * tangentVector1[0] - vVector[0] * tangentVector2[0]) * denominator;
			tangent.y = (vVector[1] * tangentVector1[1] - vVector[0] * tangentVector2[1]) * denominator;
			tangent.z = (vVector[1] * tangentVector1[2] - vVector[0] * tangentVector2[2]) * denominator;

			/*binormal.x = (uVector[0] * tangentVector2[0] - uVector[1] * tangentVector1[0]) * denominator;
			binormal.y = (uVector[0] * tangentVector2[1] - uVector[1] * tangentVector1[1]) * denominator;
			binormal.z = (uVector[0] * tangentVector2[2] - uVector[1] * tangentVector1[2]) * denominator;*/

			tangent = Normalize(tangent);
			//binormal = Normalize(binormal);

			for (int index = 0; index < 3; index++)
			{
				mesh->mTangents[mesh->mFaces[i].mIndices[index]] = tangent;
			}
		}
	}
}

}