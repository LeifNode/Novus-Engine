////================================================================
//// Copyright (c) 2015 Leif Erkenbrach
//// Distributed under the terms of the MIT License.
//// (See accompanying file LICENSE or copy at
//// http://opensource.org/licenses/MIT)
////================================================================
//
//#pragma once
//
//#ifndef NOVUS_DYNAMIC_MESH_COMPONENT_H
//#define NOVUS_DYNAMIC_MESH_COMPONENT_H
//
//#include "Core/ActorComponent.h"
//#include "Graphics/MeshRenderer.h"
//
//namespace novus
//{
//
//	class DynamicMeshComponent : public ActorComponent
//	{
//		NOVUS_OBJECT_DECL(DynamicMeshComponent);
//
//	public:
//		DynamicMeshComponent(StaticMesh* mesh);
//		~DynamicMeshComponent();
//
//		void Render(D3DRenderer* renderer) override;
//
//		void setStaticMesh(StaticMesh* mesh);
//
//		StaticMesh* getStaticMesh() const;
//
//	private:
//		MeshRenderer<* mpStaticMesh;
//	};
//
//	NOVUS_OBJECT_DEF(DynamicMeshComponent);
//
//}
//
//
//#endif