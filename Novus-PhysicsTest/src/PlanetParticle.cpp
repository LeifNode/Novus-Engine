#include "PlanetParticle.h"

#include <Graphics/LineRenderer.h>
#include <Graphics/D3DRenderer.h>

using namespace novus;

PlanetParticle::PlanetParticle()
	: mPlanetRadius(0.0),
	mTimeToUpdate(0.0f),
	mpPathRenderer(NULL)
{}


PlanetParticle::~PlanetParticle()
{
	NE_DELETE(mpPathRenderer);
}

void PlanetParticle::Init()
{
	mpPathRenderer = NE_NEW LineRenderer();
	mpPathRenderer->Init();
	mpPathRenderer->setLineWidth(0.001f);
	mpPathRenderer->setDrawMode(LineDrawMode::LineStrip);
}

void PlanetParticle::Update(float dt)
{
	Particle::Update(dt);

	static const float interval = 50000.0;

	if (mTimeToUpdate <= 0.0f)
	{
		mpPathRenderer->points.AddPoint(Vector3(getPosition() * 1.0e-7));
		mpPathRenderer->ReloadPoints();
		mTimeToUpdate = interval;
	}

	mTimeToUpdate -= dt;
}

void PlanetParticle::Render(D3DRenderer* renderer)
{
	mpPathRenderer->Render(renderer);
}