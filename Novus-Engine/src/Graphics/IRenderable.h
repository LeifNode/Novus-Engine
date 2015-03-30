//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_I_RENDERABLE_H
#define NOVUS_I_RENDERABLE_H

namespace novus
{

class D3DRenderer;

class IRenderable
{
public:
	virtual ~IRenderable() {}

	virtual void PreRender(novus::D3DRenderer* renderer) {}
	virtual void Render(novus::D3DRenderer* renderer) = 0;
	virtual void PostRender(novus::D3DRenderer* renderer) {}
};

}

#endif