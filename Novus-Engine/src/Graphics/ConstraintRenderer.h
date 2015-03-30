//================================================================
// Copyright (c) 2015 Leif Erkenbrach
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
// http://opensource.org/licenses/MIT)
//================================================================

#pragma once

#ifndef NOVUS_CONSTRAINT_RENDERER_H
#define NOVUS_CONSTRAINT_RENDERER_H

#include "IRenderable.h"
#include "LineRenderer.h"

namespace novus
{

class Particle;
class D3DRenderer;

class ConstraintRenderer : public IRenderable
{
public:
	ConstraintRenderer(float lineWidth, const Vector4& color);
	~ConstraintRenderer();

	void addConstraint(Particle* p1, Particle* p2);
	void removeConstraint(Particle* p1, Particle* p2);
	void removeConnectedConstraints(Particle* particle);

	void Update(float dt);
	void Render(D3DRenderer* renderer);

private:
	LineRenderer mLineRenderer;

	std::list<std::pair<Particle*, Particle*>> mParticleReferences;
};

}

#endif