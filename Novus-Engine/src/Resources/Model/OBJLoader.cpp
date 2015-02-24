#include "OBJLoader.h"


namespace novus
{

OBJLoader::OBJLoader()
	: mpFile(NULL),
	mpScene(NULL)
{}

OBJLoader::~OBJLoader() override
{
	NE_DELETE(mpFile);
	NE_DELETE(mpScene);
}

bool OBJLoader::Load(const std::wstring& path)
{
	NE_DELETE(mpFile);
	NE_DELETE(mpScene);

	mpFile = new std::ifstream(path);


}

assettypes::Scene* OBJLoader::getScene() const
{
	return mpScene;
}

}