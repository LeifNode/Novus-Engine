#include "MeshCalculateNormalsPass.h"
#include "Math/Math.h"

namespace novus
{

MeshCalculateNormalsPass::MeshCalculateNormalsPass(bool ignoreExistingNormals)
	: mIgnoreExistingNormals(ignoreExistingNormals)
{}

MeshCalculateNormalsPass::~MeshCalculateNormalsPass()
{}

void MeshCalculateNormalsPass::Execute(assettypes::Mesh* mesh)
{
	bool warnTriggered = false;

	if (!mesh->hasNormals())
	{
		mesh->mNormals = NE_NEW Vector3[mesh->mVertexCount];
	}
	else if (!mIgnoreExistingNormals)
		return;

	for (unsigned int face = 0; face < mesh->mFaceCount; face++)
	{
		if (mesh->mFaces[face].mIndexCount != 3 && !warnTriggered)
		{
			//NE_WARN("Normal calculation assumes that face is a triangle; normals may look incorrect.", "MeshCalculateNormalsPass");
			warnTriggered = true;
		}

		Vector3 faceNorm = Cross(Normalize(mesh->mVertices[mesh->mFaces[face].mIndices[1]] - mesh->mVertices[mesh->mFaces[face].mIndices[0]]),
								 Normalize(mesh->mVertices[mesh->mFaces[face].mIndices[2]] - mesh->mVertices[mesh->mFaces[face].mIndices[0]]));

		for (unsigned int i = 0; i < mesh->mFaces[face].mIndexCount; i++)
			mesh->mNormals[mesh->mFaces[face].mIndices[i]] = faceNorm;
	}
}

}