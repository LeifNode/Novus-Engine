#include "MassAggregateTest.h"

#include <Application/EngineStatics.h>
#include <Events/Events.h>
#include <Events/EventSystem.h>
#include <Resources/Font/Font.h>
#include <Resources/Font/FontManager.h>
#include <Math/Math.h>
#include <Graphics/Camera.h>
#include <Graphics/D3DRenderer.h>
#include <Graphics/PostProcess/DeferredRenderer.h>
#include <Graphics/Textures/Texture2D.h>
#include <Graphics/LineRenderer.h>
#include <Graphics/SkyboxRenderer.h>
#include <Physics/Particle.h>
#include <Physics/PhysicsSystem.h>

using namespace novus;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd){
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	MassAggregateTest* application = NE_NEW MassAggregateTest(hInstance);

	if (!application->Init())
		return 0;

	int returnVal = application->Run();

	NE_DELETE(application);

	return returnVal;
}

MassAggregateTest::MassAggregateTest(HINSTANCE instance)
:
NovusApplication(instance),
mpMainShader(NULL),
mpSkyboxRenderer(NULL),
mpPhysicsSystem(NULL),
mpRodRenderer(NULL),
mpCableRenderer(NULL),
mpSpringRenderer(NULL),
mpGravityForce(NULL)
{
	mMainWndCaption = L"Novus Engine Material Test v0.1.66";

	mpCamera = NE_NEW Camera();
	mpCamera->setPosition(Vector3(0.0f, 4.9f, 1.4f));
	mpCamera->setVelocity(3.0f);

	mpCamera->setPosition(Vector3(3.0f, 2.0f, 3.0f) * 10.0f);
	mpCamera->LookAt(Vector3(0.0f, 0.0f, 0.0f));
	//mpCamera->setRotation(Quaternion::AxisAngle((Normalize(Vector3(0.5f, 1.0f, 0.0f))), Math::Pi * 0.3f));
}

MassAggregateTest::~MassAggregateTest()
{
	UnhookInputEvents();
	NE_DELETE(mpCamera);
	NE_DELETE(mpSkyboxRenderer);
	NE_DELETE(mpPhysicsSystem);
	NE_DELETE(mpRodRenderer);
	NE_DELETE(mpCableRenderer);
	NE_DELETE(mpSpringRenderer);
}

bool MassAggregateTest::Init()
{
	if (!NovusApplication::Init())
		return false;

	OnResize();

	HookInputEvents();

	InitShaders();
	InitMesh();

	novus::Font* verdana = mpFontManager->LoadFont(
		"verdana",
		"../Fonts/verdana.ttf",
		"../Fonts/verdanab.ttf",
		"../Fonts/verdanai.ttf",
		"../Fonts/verdanaz.ttf");

	verdana->LoadGlyphs(24, novus::FontType::Normal);
	verdana->LoadGlyphs(24, novus::FontType::Bold);
	verdana->LoadGlyphs(24, novus::FontType::Italic);
	verdana->LoadGlyphs(24, novus::FontType::BoldItalic);

	mpSkyboxRenderer = NE_NEW SkyboxRenderer();
	mpSkyboxRenderer->Init(L"../Textures/sunsetcube1024.dds");

	mpPhysicsSystem = NE_NEW PhysicsSystem();

	mpPhysicsSystem->Init();

	InitPhysicsActors();

	mpRodRenderer = NE_NEW LineRenderer();
	mpRodRenderer->Init();
	mpRodRenderer->setLineWidth(0.05f);
	mpRodRenderer->setLineColor(Vector4(1.0f, 0.5f, 0.0f, 1.0f));

	mpCableRenderer = NE_NEW LineRenderer();
	mpCableRenderer->Init();
	mpCableRenderer->setLineWidth(0.05f);
	mpCableRenderer->setLineColor(Vector4(0.0f, 1.0f, 0.0f, 1.0f));

	mpSpringRenderer = NE_NEW LineRenderer();
	mpSpringRenderer->Init();
	mpSpringRenderer->setLineWidth(0.05f);
	mpSpringRenderer->setLineColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));

	return true;
}

void MassAggregateTest::InitShaders()
{
	ShaderInfo shaderInfo[] = {
		{ ShaderType::Vertex, "VS" },
		{ ShaderType::Pixel, "PS" },
		{ ShaderType::None, NULL }
	};

	D3D11_INPUT_ELEMENT_DESC vertexDescription[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	mpMainShader = mpRenderer->LoadShader(L"../Shaders/GenericShader.hlsl", shaderInfo, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, vertexDescription, ARRAYSIZE(vertexDescription));
}

void MassAggregateTest::InitMesh()
{
	Mesh mesh;

	GeometryGenerator::CreateGeosphere(0.2f, 4, mesh);

	mMeshRenderer.Init(mpRenderer, mesh.Vertices, mesh.Indices);

	GeometryGenerator::CreatePlane(1000.0f, 1000.0f, 4, 4, mesh);

	mPlaneRenderer.Init(mpRenderer, mesh.Vertices, mesh.Indices);
}

void MassAggregateTest::InitPhysicsActors()
{
	float particleRadius = 0.2f;

	mpGravityForce = NE_NEW ParticleGravity(Vector3(0.0f, -9.8f, 0.0f));

	mpPhysicsSystem->AddForceGenerator(mpGravityForce);

	InitPhysicsBounds(particleRadius);

	ParticleSelfCollision* selfCollider = NE_NEW ParticleSelfCollision(particleRadius);
	selfCollider->pParticles = mpPhysicsSystem->getParticles();

	mpPhysicsSystem->AddContactGenerator(selfCollider);
}

void MassAggregateTest::InitPhysicsBounds(float particleRadius)
{
	float bounds = 20.0f;

	//Ground
	ParticlePlane* planeCollider = NE_NEW ParticlePlane(Vector3(0.0f), Vector3(0.0f, 1.0f, 0.0f), particleRadius);
	planeCollider->pParticles = mpPhysicsSystem->getParticles();
	planeCollider->restitution = 1.0f;

	mpPhysicsSystem->AddContactGenerator(planeCollider);

	//Ceiling
	planeCollider = NE_NEW ParticlePlane(Vector3(0.0f, bounds, 0.0f), Vector3(0.0f, -1.0f, 0.0f), particleRadius);
	planeCollider->pParticles = mpPhysicsSystem->getParticles();
	planeCollider->restitution = 1.0f;

	mpPhysicsSystem->AddContactGenerator(planeCollider);

	//Walls
	planeCollider = NE_NEW ParticlePlane(Vector3(bounds, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), particleRadius);
	planeCollider->pParticles = mpPhysicsSystem->getParticles();
	planeCollider->restitution = 1.0f;
	mpPhysicsSystem->AddContactGenerator(planeCollider);

	planeCollider = NE_NEW ParticlePlane(Vector3(-bounds, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), particleRadius);
	planeCollider->pParticles = mpPhysicsSystem->getParticles();
	planeCollider->restitution = 1.0f;
	mpPhysicsSystem->AddContactGenerator(planeCollider);

	planeCollider = NE_NEW ParticlePlane(Vector3(0.0f, 0.0f, bounds), Vector3(0.0f, 0.0f, -1.0f), particleRadius);
	planeCollider->pParticles = mpPhysicsSystem->getParticles();
	planeCollider->restitution = 1.0f;
	mpPhysicsSystem->AddContactGenerator(planeCollider);

	planeCollider = NE_NEW ParticlePlane(Vector3(0.0f, 0.0f, -bounds), Vector3(0.0f, 0.0f, 1.0f), particleRadius);
	planeCollider->pParticles = mpPhysicsSystem->getParticles();
	planeCollider->restitution = 1.0f;
	mpPhysicsSystem->AddContactGenerator(planeCollider);
}

void MassAggregateTest::CreateMassAggregateBox(float pointMass, const Vector3& position, const Vector3& scale, Particle** pArrayOut)
{
	Particle* particleArray[8];

	for (int x = 0; x < 2; x++)
	{
		for (int y = 0; y < 2; y++)
		{
			for (int z = 0; z < 2; z++)
			{
				Particle* boxParticle = NE_NEW Particle();
				boxParticle->setMass(pointMass);
				boxParticle->setDamping(0.8f);
				
				boxParticle->setPosition(Vector3((float)x, (float)y, (float)z) * scale + position);

				mpPhysicsSystem->AddParticle(boxParticle);
				mpPhysicsSystem->AddRegistryEntry(boxParticle, mpGravityForce);

				particleArray[x * 4 + y * 2 + z] = boxParticle;

				if (pArrayOut != NULL)
					pArrayOut[x * 4 + y * 2 + z] = boxParticle;
			}
		}
	}

	for (int x = 0; x < 2; x++)
	{
		for (int y = 0; y < 2; y++)
		{
			AddRod(particleArray[x * 4 + y * 2], particleArray[x * 4 + y * 2 + 1]);
		}
	}

	for (int x = 0; x < 2; x++)
	{
		for (int z = 0; z < 2; z++)
		{
			AddRod(particleArray[x * 4 + z], particleArray[x * 4 + z + 2]);
		}
	}

	for (int y = 0; y < 2; y++)
	{
		for (int z = 0; z < 2; z++)
		{
			AddRod(particleArray[y * 2 + z], particleArray[y * 2 + z + 4]);
		}
	}

	AddRod(particleArray[0], particleArray[7]);
	AddRod(particleArray[1], particleArray[6]);
	AddRod(particleArray[2], particleArray[5]);
	AddRod(particleArray[3], particleArray[4]);

	AddRod(particleArray[0], particleArray[3]);
	AddRod(particleArray[4], particleArray[7]);
	AddRod(particleArray[2], particleArray[7]);
	AddRod(particleArray[1], particleArray[4]);
	AddRod(particleArray[2], particleArray[4]);
	AddRod(particleArray[1], particleArray[7]);
}

void MassAggregateTest::CreateMassAggregateTetrahedron(float pointMass, const novus::Vector3& position, const novus::Vector3& scale, Particle** pArrayOut)
{
	Particle* particleArray[4];

	for (int i = 0; i < 4; i++)
	{
		Particle* tetraParticle = NE_NEW Particle();
		tetraParticle->setMass(pointMass);
		tetraParticle->setDamping(0.8f);

		mpPhysicsSystem->AddParticle(tetraParticle);
		mpPhysicsSystem->AddRegistryEntry(tetraParticle, mpGravityForce);

		particleArray[i] = tetraParticle;

		if (pArrayOut != NULL)
		{
			pArrayOut[i] = tetraParticle;
		}
	}

	particleArray[0]->setPosition(Vector3(1.0f, 0.0f, -0.7071f) * scale + position);
	particleArray[1]->setPosition(Vector3(-1.0f, 0.0f, -0.7071f) * scale + position);
	particleArray[2]->setPosition(Vector3(0.0f, 1.0f, 0.7071f) * scale + position);
	particleArray[3]->setPosition(Vector3(0.0f, -1.0f, 0.7071f) * scale + position);

	//Base
	AddRod(particleArray[0], particleArray[1]);
	AddRod(particleArray[1], particleArray[2]);
	AddRod(particleArray[2], particleArray[0]);

	//Sides
	AddRod(particleArray[0], particleArray[3]);
	AddRod(particleArray[1], particleArray[3]);
	AddRod(particleArray[2], particleArray[3]);
}

void MassAggregateTest::CreateMassAggregatePyramid(float pointMass, const novus::Vector3& position, const novus::Vector3& scale, Particle** pArrayOut)
{
	Particle* particleArray[5];

	for (int i = 0; i < 5; i++)
	{
		Particle* pyramidParticle = NE_NEW Particle();
		pyramidParticle->setMass(pointMass);
		pyramidParticle->setDamping(0.8f);

		mpPhysicsSystem->AddParticle(pyramidParticle);
		mpPhysicsSystem->AddRegistryEntry(pyramidParticle, mpGravityForce);

		particleArray[i] = pyramidParticle;

		if (pArrayOut != NULL)
		{
			pArrayOut[i] = pyramidParticle;
		}
	}

	particleArray[0]->setPosition(Vector3(1.0f, 0.0f, -1.0f) * scale + position);
	particleArray[1]->setPosition(Vector3(-1.0f, 0.0f, -1.0f) * scale + position);
	particleArray[2]->setPosition(Vector3(1.0f, 0.0f, 1.0f) * scale + position);
	particleArray[3]->setPosition(Vector3(-1.0f, 0.0f, 1.0f) * scale + position);
	particleArray[4]->setPosition(Vector3(0.0f, 1.0f, 0.0f) * scale + position);

	//Base
	AddRod(particleArray[0], particleArray[1]);
	AddRod(particleArray[1], particleArray[2]);
	AddRod(particleArray[2], particleArray[3]);
	AddRod(particleArray[3], particleArray[0]);

	AddRod(particleArray[0], particleArray[2]);
	AddRod(particleArray[1], particleArray[3]);

	//Sides
	AddRod(particleArray[0], particleArray[4]);
	AddRod(particleArray[1], particleArray[4]);
	AddRod(particleArray[2], particleArray[4]);
	AddRod(particleArray[3], particleArray[4]);
}

void MassAggregateTest::CreateMassAggregateChandelier(float pointMass, const novus::Vector3& position, const novus::Vector3& scale)
{
	Particle* rootParticle = NE_NEW Particle();
	rootParticle->setInverseMass(0.0f); //set as immobile
	rootParticle->setPosition(position);
	rootParticle->setDamping(0.0f);
	mpPhysicsSystem->AddParticle(rootParticle);

	Particle* pyramidArr[5];

	CreateMassAggregatePyramid(pointMass * 5.0f, position - Vector3(0.0f, 5.0f, 0.0f), scale, pyramidArr);

	AddCable(rootParticle, pyramidArr[4]);

	for (int i = 0; i < 4; i++)
	{
		Particle* hangingParticle = NE_NEW Particle();

		hangingParticle->setMass(pointMass);
		hangingParticle->setPosition(pyramidArr[i]->getPosition() - Vector3(0.0f, 1.2f, 0.0f));
		hangingParticle->setDamping(0.7f);

		mpPhysicsSystem->AddParticle(hangingParticle);
		mpPhysicsSystem->AddRegistryEntry(hangingParticle, mpGravityForce);

		AddSpring(pyramidArr[i], hangingParticle, 15.0f);
	}
}

void MassAggregateTest::AddRod(Particle* p1, Particle* p2, float length)
{
	ParticleRod* newRod = NE_NEW ParticleRod();

	if (length == 0.0f)
	{
		newRod->length = Length(p1->getPosition() - p2->getPosition());
	}
	else
	{
		newRod->length = length;
	}

	newRod->particle[0] = p1;
	newRod->particle[1] = p2;

	mpPhysicsSystem->AddContactGenerator(newRod);
}

void MassAggregateTest::AddCable(Particle* p1, Particle* p2, float length)
{
	ParticleCable* newCable = NE_NEW ParticleCable();

	if (length == 0.0f)
	{
		newCable->maxLength = Length(p1->getPosition() - p2->getPosition());
	}
	else
	{
		newCable->maxLength = length;
	}

	newCable->particle[0] = p1;
	newCable->particle[1] = p2;

	mpPhysicsSystem->AddContactGenerator(newCable);
}

void MassAggregateTest::AddSpring(Particle* p1, Particle* p2, float springConstant, float length)
{
	ParticleSpring* newSpring1;
	ParticleSpring* newSpring2;

	if (length == 0.0f)
	{
		newSpring1 = NE_NEW ParticleSpring(p2, springConstant, Length(p1->getPosition() - p2->getPosition()));
		newSpring2 = NE_NEW ParticleSpring(p1, springConstant, Length(p1->getPosition() - p2->getPosition()));
	}
	else
	{
		newSpring1 = NE_NEW ParticleSpring(p2, springConstant, length);
		newSpring2 = NE_NEW ParticleSpring(p1, springConstant, length);
	}

	mpPhysicsSystem->AddForceGenerator(newSpring1);
	mpPhysicsSystem->AddForceGenerator(newSpring2);
	mpPhysicsSystem->AddRegistryEntry(p1, newSpring1);
	mpPhysicsSystem->AddRegistryEntry(p2, newSpring2);
}

void MassAggregateTest::ResetPhysicsSimulation()
{
	mpPhysicsSystem->Clear();

	InitPhysicsActors();
}

void MassAggregateTest::HookInputEvents()
{
	EngineStatics::getEventSystem()->AddListener(EventData_KeyboardDown::skEventType, fastdelegate::MakeDelegate(this, &MassAggregateTest::OnKeyDown));
}

void MassAggregateTest::UnhookInputEvents()
{
	EngineStatics::getEventSystem()->RemoveListener(EventData_KeyboardDown::skEventType, fastdelegate::MakeDelegate(this, &MassAggregateTest::OnKeyDown));
}

void MassAggregateTest::OnKeyDown(novus::IEventDataPtr eventData)
{
	auto dataPtr = static_pointer_cast<EventData_KeyboardDown>(eventData);

	float scale;

	switch (dataPtr->getKey())
	{
	case KeyboardKey::KEY_ESC:
		ExitApp();
		break;
	case KeyboardKey::KEY_R:
		ResetPhysicsSimulation();
		break;
	case KeyboardKey::KEY_LEFT:
		mpGravityForce->setGravity(Vector3(9.8f, 0.0f, 0.0f));
		break;
	case KeyboardKey::KEY_RIGHT:
		mpGravityForce->setGravity(Vector3(-9.8f, 0.0f, 0.0f));
		break;
	case KeyboardKey::KEY_UP:
		mpGravityForce->setGravity(Vector3(0.0f, 9.8f, 0.0f));
		break;
	case KeyboardKey::KEY_DOWN:
		mpGravityForce->setGravity(Vector3(0.0f, -9.8f, 0.0f));
		break;
	case KeyboardKey::KEY_SPACE:
		mpGravityForce->setGravity(Vector3(0.0f, 0.0f, 0.0f));
		break;
	case KeyboardKey::KEY_Z:
		scale = Math::RandF(0.5f, 1.0f);
		CreateMassAggregateBox(scale, Vector3(Math::RandF(-15.0f, 15.0f), Math::RandF(0.0f, 10.0f), Math::RandF(-15.0f, 15.0f)), Vector3(scale));
		break;
	case KeyboardKey::KEY_X:
		scale = Math::RandF(0.5f, 1.0f);
		CreateMassAggregateTetrahedron(scale, Vector3(Math::RandF(-15.0f, 15.0f), Math::RandF(0.0f, 40.0f), Math::RandF(-15.0f, 15.0f)), Vector3(scale));
		break;
	case KeyboardKey::KEY_C:
		scale = Math::RandF(0.5f, 1.0f);
		CreateMassAggregatePyramid(scale, Vector3(Math::RandF(-15.0f, 15.0f), Math::RandF(0.0f, 15.0f), Math::RandF(-15.0f, 15.0f)), Vector3(scale));
		break;
	case KeyboardKey::KEY_V:
		scale = Math::RandF(1.0f, 1.0f);
		CreateMassAggregateChandelier(scale, Vector3(Math::RandF(-10.0f, 10.0f), 15.0f, Math::RandF(-10.0f, 10.0f)), Vector3(scale));
		break;
	}
}

void MassAggregateTest::OnResize()
{
	NovusApplication::OnResize();

	mpCamera->OnResize(getClientWidth(), getClientHeight());
}

void MassAggregateTest::Update(float dt)
{
	//mpCamera->LookAt(Vector3(0.0f, -5.0f, 0.0f));
	mpCamera->Update(dt);
	//mpCamera->LookAt(Vector3(0.0f, -5.0f, 0.0f));

	mpRenderer->getDeferredRenderer()->Update(dt);

	mpPhysicsSystem->Update(dt);
	
	UpdateContactRenderers();
}

void MassAggregateTest::UpdateContactRenderers()
{
	mpRodRenderer->points.Clear();
	mpCableRenderer->points.Clear();
	mpSpringRenderer->points.Clear();

	for (auto it = mpPhysicsSystem->getContactGenerators()->cbegin(); it != mpPhysicsSystem->getContactGenerators()->cend(); ++it)
	{
		ParticleRod* rod = dynamic_cast<ParticleRod*>(*it);

		if (rod != NULL)
		{
			mpRodRenderer->points.AddPoint(rod->particle[0]->getPosition());
			mpRodRenderer->points.AddPoint(rod->particle[1]->getPosition());
		}
		
		ParticleCable* cable = dynamic_cast<ParticleCable*>(*it);

		if (cable != NULL)
		{
			mpCableRenderer->points.AddPoint(cable->particle[0]->getPosition());
			mpCableRenderer->points.AddPoint(cable->particle[1]->getPosition());
		}
	}

	for (auto it = mpPhysicsSystem->getForceRegistry()->getRegistrations()->cbegin(); it != mpPhysicsSystem->getForceRegistry()->getRegistrations()->cend(); ++it)
	{
		ParticleSpring* spring = dynamic_cast<ParticleSpring*>(it->fg);

		if (spring != NULL)
		{
			mpSpringRenderer->points.AddPoint(it->particle->getPosition());
			mpSpringRenderer->points.AddPoint(spring->getOther()->getPosition());
		}
	}

	mpRodRenderer->ReloadPoints();
	mpCableRenderer->ReloadPoints();
	mpSpringRenderer->ReloadPoints();
}

void MassAggregateTest::Render()
{
	mpRenderer->PreRender();
	mpRenderer->setShader(mpMainShader);

	CBPerFrame perFrame;

	perFrame.ScreenResolution = Vector2_t<unsigned int>(
		static_cast<unsigned int>(getClientWidth()),
		static_cast<unsigned int>(getClientHeight()));
	perFrame.ClipNearFar = Vector2(mpCamera->getNear(), mpCamera->getFar());
	perFrame.Projection = mpCamera->getProj();
	perFrame.ProjectionInv = Matrix4::Inverse(perFrame.Projection);
	perFrame.View = mpCamera->getView();
	perFrame.ViewInv = Matrix4::Inverse(perFrame.View);
	perFrame.ViewProj = perFrame.View * perFrame.Projection;
	perFrame.ViewProjInv = Matrix4::Inverse(perFrame.ViewProj);
	perFrame.EyePosition = mpCamera->getPosition();

	mpRenderer->setPerFrameBuffer(perFrame);
	mpRenderer->BindPerFrameBuffer();

	CBPerObject perObject;
	perObject.Material.HasDiffuseTexture = false;
	perObject.Material.HasNormalTexture = false;
	perObject.Material.HasSpecularTexture = false;

	int particleCount = mpPhysicsSystem->getParticles()->size();
	int count = 0;
	for (auto it = mpPhysicsSystem->getParticles()->cbegin(); it != mpPhysicsSystem->getParticles()->cend(); ++it)
	{
		perObject.World = Matrix4::Translate((*it)->getPosition());
		perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
		perObject.WorldViewProj = perObject.World * perFrame.ViewProj;
		perObject.Material.Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		perObject.Material.Emissive = Vector3(0.0f, 0.0f, 0.0f);
		perObject.Material.SpecularColor = Vector3(0.725f, 0.58f, 0.271f);
		perObject.Material.Metallic = 0.0f;
		perObject.Material.Roughness = 0.15f;
		mpRenderer->setPerObjectBuffer(perObject);

		mMeshRenderer.Render(mpRenderer);

		count++;
	}

	perObject.World = Matrix4::Translate(0.0f, 0.0f, 0.0f);
	perObject.WorldInvTranspose = Matrix4::Transpose(Matrix4::Inverse(perObject.World));
	perObject.WorldViewProj = perObject.World * perFrame.ViewProj;
	perObject.Material.Diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	perObject.Material.SpecularColor = Vector3(1.0f, 1.0f, 1.0f);
	perObject.Material.Emissive = Vector3(0.0f);
	perObject.Material.Metallic = 0.0f;
	perObject.Material.Roughness = 0.4f;
	mpRenderer->setPerObjectBuffer(perObject);
	mPlaneRenderer.Render(mpRenderer);


	mpSkyboxRenderer->Render(mpRenderer);

	mpRodRenderer->Render(mpRenderer);
	mpCableRenderer->Render(mpRenderer);
	mpSpringRenderer->Render(mpRenderer);

	mpRenderer->RenderDeferredShading();

	mpRenderer->getDeferredRenderer()->RenderDebugOutput(mpRenderer);

	mpRenderer->PostRender();
}