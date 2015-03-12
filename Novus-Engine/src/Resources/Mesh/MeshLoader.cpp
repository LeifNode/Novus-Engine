#include "MeshLoader.h"

namespace novus
{

MeshLoader::MeshLoader()
	: mpScene(NULL)
{}

MeshLoader::~MeshLoader()
{
	NE_DELETE(mpScene);
}

void MeshLoader::DeleteScene()
{
	NE_DELETE(mpScene);
}

}