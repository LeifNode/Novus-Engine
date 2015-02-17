#include "PlanetParticle.h"
#include "PhysicsTestApp.h"

#include <Graphics/LineRenderer.h>
#include <Graphics/D3DRenderer.h>
#include <Graphics/TextRenderer.h>
#include <Application/EngineStatics.h>
#include <Resources/Font/FontManager.h>

using namespace novus;

PlanetParticle::PlanetParticle()
	: mPlanetRadius(0.0),
	mTimeToUpdate(0.0f),
	mpPathRenderer(NULL),
	mpNameRenderer(NULL),
	mpForceVector(NULL),
	mpVelocityVector(NULL)
{}


PlanetParticle::~PlanetParticle()
{
	NE_DELETE(mpPathRenderer);
	NE_DELETE(mpNameRenderer);
	NE_DELETE(mpForceVector);
	NE_DELETE(mpVelocityVector);
}

void PlanetParticle::Init()
{
	mpPathRenderer = NE_NEW LineRenderer();
	mpPathRenderer->Init();
	mpPathRenderer->setLineWidth(0.001f);
	mpPathRenderer->setDrawMode(LineDrawMode::LineStrip);

	mpForceVector = NE_NEW LineRenderer();
	mpForceVector->Init();
	mpForceVector->setLineWidth(0.0001f);
	mpForceVector->setDrawMode(LineDrawMode::LineStrip);
	mpForceVector->setLineColor(Vector4(0.0f, 1.0f, 0.0f, 1.0));

	mpVelocityVector = NE_NEW LineRenderer();
	mpVelocityVector->Init();
	mpVelocityVector->setLineWidth(0.0001f);
	mpVelocityVector->setDrawMode(LineDrawMode::LineStrip);
	mpVelocityVector->setLineColor(Vector4(0.0f, 0.0f, 1.0f, 1.0));

	mpNameRenderer = NE_NEW TextRenderer(50);
	mpNameRenderer->Init();
	mpNameRenderer->setFont(EngineStatics::getFontManager()->GetFont("sansserif"));
	mpNameRenderer->setTextSize(50);
	mpNameRenderer->setText(mPlanetName);
	mpNameRenderer->mTransform.SetPosition(Vector3(0.0f, 0.0f, 0.0f));
}

void PlanetParticle::Update(float dt)
{
	Vector3d acc = getAcceleration();
	Particle::Update(dt);
	
	if (Length(acc) > 0.001)
	{
		Vector3d accEnd = Vector3d(getPosition() * RENDERER_SCALING + acc * RENDERER_SCALING * RENDERER_SCALING * 0.0000001);

		mpForceVector->points.Clear();
		mpForceVector->points.AddPoint(Vector3(getPosition() * RENDERER_SCALING));
		mpForceVector->points.AddPoint(Vector3(accEnd));
	}

	if (LengthSq(mVelocity) > 0.001)
	{
		mpVelocityVector->points.Clear();
		mpVelocityVector->points.AddPoint(Vector3(getPosition() * RENDERER_SCALING));
		mpVelocityVector->points.AddPoint(Vector3(getPosition() * RENDERER_SCALING + mVelocity * 0.001));
	}

	static const float interval = 50000.0;

	if (mTimeToUpdate <= 0.0f)
	{
		mpPathRenderer->points.AddPoint(Vector3(getPosition() * RENDERER_SCALING));

		if (mpPathRenderer->points.list.size() > 500)
		{
			std::vector<Vector3> culledList;
			for (int i = 100; i < mpPathRenderer->points.list.size(); i++)
				culledList.push_back(mpPathRenderer->points.list[i]);

			mpPathRenderer->points.setPoints(culledList);
		}

		mpPathRenderer->ReloadPoints();
		mTimeToUpdate = interval;
	}

	mTimeToUpdate -= dt;
}

void PlanetParticle::Render(D3DRenderer* renderer)
{
	mpPathRenderer->Render(renderer);

	mpForceVector->ReloadPoints();
	mpForceVector->Render(renderer);

	mpVelocityVector->ReloadPoints();
	mpVelocityVector->Render(renderer);
}

void PlanetParticle::ForwardRender(novus::D3DRenderer* renderer)
{
	CBPerObject perObject;

	Vector3 scaledPosition = Vector3(mPosition * RENDERER_SCALING);

	float scale = 3.0f * Math::Min(100.0f, Length(scaledPosition - renderer->getPerFrameBuffer()->EyePosition));
	mpNameRenderer->mTransform.SetScale(scale);

	//perObject.World =  Matrix4::Scale(1.0f, 1.0f, 1.0f) * mTransform.GetTransform();
	perObject.World = mpNameRenderer->mTransform.GetTransform() * Matrix4::Translate(scaledPosition + Vector3(0.0f, mPlanetRadius * RENDERER_SCALING + 0.0f, 0.0f));
	perObject.WorldInvTranspose = Matrix4::Inverse(Matrix4::Transpose(perObject.World));
	perObject.WorldViewProj = perObject.World * renderer->getPerFrameBuffer()->ViewProj;

	renderer->setPerObjectBuffer(perObject);

	mpNameRenderer->Render(renderer);
}