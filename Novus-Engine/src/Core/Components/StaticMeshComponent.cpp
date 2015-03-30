#include "StaticMeshComponent.h"

namespace novus
{

StaticMeshComponent::StaticMeshComponent(StaticMesh* mesh)
:ActorComponent(),
mpStaticMesh(mesh)
{
}

StaticMeshComponent::~StaticMeshComponent()
{
}

void StaticMeshComponent::Render(D3DRenderer* renderer)
{
	mpStaticMesh->Render(renderer);
}

void StaticMeshComponent::setStaticMesh(StaticMesh* mesh)
{
	mpStaticMesh = mesh;
}

StaticMesh* StaticMeshComponent::getStaticMesh() const
{
	return mpStaticMesh;
}

}