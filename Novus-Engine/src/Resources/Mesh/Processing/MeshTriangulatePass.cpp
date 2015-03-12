#include "MeshTriangulatePass.h"

namespace novus
{

MeshTriangulatePass::MeshTriangulatePass()
{}

MeshTriangulatePass::~MeshTriangulatePass()
{}

void MeshTriangulatePass::Execute(assettypes::Mesh* mesh)
{
	unsigned int faceCount = mesh->mFaceCount;
	assettypes::Face* faces = mesh->mFaces;

	std::vector<assettypes::Face> newFaces;
	newFaces.reserve(faceCount); //We will always have at least this many faces

	for (unsigned int face = 0; face < faceCount; face++)
	{
		if (faces[face].mIndexCount == 3)
			newFaces.push_back(faces[face]);
		else if (faces[face].mIndexCount == 4)
		{
			assettypes::Face f1;
			assettypes::Face f2;

			f1.mIndexCount = 3;
			f2.mIndexCount = 3;

			f1.mIndices[0] = faces[face].mIndices[0];
			f1.mIndices[1] = faces[face].mIndices[1];
			f1.mIndices[2] = faces[face].mIndices[2];
			f1.mIndices[3] = 0;
			
			f2.mIndices[0] = faces[face].mIndices[3];
			f2.mIndices[1] = faces[face].mIndices[0];
			f2.mIndices[2] = faces[face].mIndices[2];
			f2.mIndices[3] = 0;

			newFaces.push_back(f1);
			newFaces.push_back(f2);
		}
		else
		{
			NE_ERROR("Invalid face count.", "MeshTriangulatePass");
			return;
		}
	}

	if (newFaces.size() != faceCount)//Part of this mesh was triangulated
	{
		NE_DELETEARR(mesh->mFaces);

		mesh->mFaceCount = static_cast<unsigned int>(newFaces.size());
		mesh->mFaces = NE_NEW assettypes::Face[newFaces.size()];
		memcpy(mesh->mFaces, &newFaces[0], newFaces.size() * sizeof(assettypes::Face));
		//std::copy(newFaces.begin(), newFaces.end(), mesh->mFaces); 
	}
}

}