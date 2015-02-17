#include "PlanetUIRenderer.h"
#include "PlanetParticle.h"
#include <Graphics/D3DRenderer.h>
#include <Graphics/TextRenderer.h>
#include <Math/Math.h>
#include <Application/Common.h>
#include <Application/EngineStatics.h>
#include <Application/NovusApplication.h>
#include <Resources/Font/FontManager.h>

using namespace novus;

PlanetUIRenderer::PlanetUIRenderer()
	: mpTextRenderer(NULL)
{

}

PlanetUIRenderer::~PlanetUIRenderer()
{
	NE_DELETE(mpTextRenderer);
}

void PlanetUIRenderer::Init()
{
	mpTextRenderer = NE_NEW novus::TextRenderer(400);
	mpTextRenderer->Init();
	mpTextRenderer->setTextSize(16);
	mpTextRenderer->setFont(EngineStatics::getFontManager()->GetFont("sansserif"));
	mpTextRenderer->setVerticalSpacing(0.008f);
	mpTextRenderer->mTransform.SetPosition(Vector3(0.18f, 0.245f, 0.0f));
	mpTextRenderer->mTransform.SetScale(4.0f, 4.0f, 4.0f);
	SelectPlanet(NULL);
}

void PlanetUIRenderer::SelectPlanet(const PlanetParticle* planet)
{
	std::stringstream stream;

	if (planet != NULL)
	{
		stream << "Planet:           " << planet->getName() << "\n"
			   << "Mass (kg):      " << planet->getMass() << "\n"
			   << "Speed (km/s): " << Length(planet->getVelocity()) << "\n"
			   << "Velocity (km/s):\nX: " << planet->getVelocity().x << "\nY: "
			   << planet->getVelocity().y << "\nZ: " << planet->getVelocity().z << "\n";
	}
	else
	{
		stream << "No planet selected";
	}

	mpTextRenderer->setText(stream.str());
}

void PlanetUIRenderer::Render(novus::D3DRenderer* renderer)
{
	float aspectRatio = (float)EngineStatics::getApplication()->getClientHeight() / (float)EngineStatics::getApplication()->getClientWidth();

	CBPerObject perObject;

	//perObject.World =  Matrix4::Scale(1.0f, 1.0f, 1.0f) * mTransform.GetTransform();
	perObject.World = mpTextRenderer->mTransform.GetTransform();
	perObject.WorldInvTranspose = Matrix4::Inverse(Matrix4::Transpose(perObject.World));
	perObject.WorldViewProj = perObject.World * Matrix4::Orthographic(1.0f, aspectRatio, 0.0f, 1.0f);

	renderer->setPerObjectBuffer(perObject);

	mpTextRenderer->Render(renderer);
}