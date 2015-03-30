//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_I_RENDER_TARGET_H
#define NOVUS_I_RENDER_TARGET_H

namespace novus
{

class D3DRenderer;
class Actor;

namespace RenderPass
{
	enum Type
	{
		Depth, //Depth prepass for forward+; no dependencies
		Shadow, //Depth pass for shadows affected by whether objects are marked as shadow casters; no dependencies
		GraphicsPrepass, //Voxelization and reflection probe generation; depends on depth data for shadowing
		RenderTargets, //Cameras in the scene that generate texture used later in the pipeline. Video monitors for example.
		Final, //GBuffer and forward rendering passes; depends on all previous passes
	};
}

class IRenderTarget
{
public:
	//Binds target and any resources it requires
	virtual void BindTarget(D3DRenderer* renderer) = 0;
	virtual void UnbindTarget(D3DRenderer* renderer) = 0;

	virtual RenderPass::Type GetRenderPass() const = 0;

	virtual bool ShouldRender(const Actor* actor) const = 0;
};

}


#endif