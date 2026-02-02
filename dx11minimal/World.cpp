////////////////////////////////////////////////////////////////////////////////
// Filename: World.cpp
////////////////////////////////////////////////////////////////////////////////
#include "world.h"


World::World()
{
}


World::World(const World& other)
{
}


World::~World()
{
}


bool World::Initialize(float iaspect)
{
	m_Camera = new CameraClass;
	m_Camera->Initialize(iaspect);
	m_Camera->SetPosition(point3d(0.0f, 0.0f, -10.0f));

	entityStorage = new EntityStorage;
	entityStorage->Initialize();

	return true;
}


void World::Shutdown()
{
	size_t size = physicSystems.size();
	for (int i = 0; i < size; i++)
	{
		physicSystems[i]->Shutdown();
	}

	size = renderSystems.size();
	for (int i = 0; i < size; i++)
	{
		renderSystems[i]->Shutdown();
	}

	if (entityStorage) {
		entityStorage->Shutdown();
		entityStorage = 0;
	}
}


void World::PreCalculations()
{
	InputAssembler::IA(InputAssembler::topology::triList);
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Depth::Depth(Depth::depthmode::off);
	Rasterizer::Cull(Rasterizer::cullmode::off);

	Textures::RenderTarget(11, 0);
	Shaders::vShader(10);
	Shaders::pShader(200);
	Draw::Clear({ 0.0f, 0.0f, 0.0f, 1.0f });
	context->Draw(6, 0);
	Textures::CreateMipMap();

	Textures::RenderTarget(12, 0);
	Shaders::pShader(201);
	Draw::Clear({ 0.0f, 0.0f, 0.0f, 1.0f });
	context->Draw(6, 0);
	Textures::CreateMipMap();

	Textures::RenderTarget(1, 0);
}


void World::UpdatePhysic()
{
	double deltaTime = timer::deltaTime / 1000;
	size_t size = physicSystems.size();
	for (int i = 0; i < size; i++)
	{
		physicSystems[i]->Update(entityStorage->entities, deltaTime);
	}
	entityStorage->CleanMem();
}


void World::UpdateRender()
{
	// Updating time in const buffer
	ConstBuf::frame.time = XMFLOAT4{ (float)timer::frameBeginTime * 0.01f, 0, 0, 0 };
	ConstBuf::UpdateFrame();
	ConstBuf::ConstToVertex(4);
	ConstBuf::ConstToGeometry(4);
	ConstBuf::ConstToPixel(4);

	if (firstFrame) {
		firstFrame = false;
		PreCalculations();
	}
	Textures::TextureToShader(11, 1);
	Textures::TextureToShader(12, 2);

	Textures::RenderTarget(1, 0);
	// Clear the buffers to begin the scene.
	Draw::Clear({ 0.0f, 0.0588f, 0.1176f, 1.0f });
	Draw::ClearDepth();

	InputAssembler::IA(InputAssembler::topology::triList);
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Depth::Depth(Depth::depthmode::off);
	Rasterizer::Cull(Rasterizer::cullmode::off);

	m_Camera->Render();

	double deltaTime = timer::deltaTime / 1000;
	size_t size = renderSystems.size();
	for (int i = 0; i < size; i++)
	{
		renderSystems[i]->Update(entityStorage->entities, deltaTime);
	}

	Textures::CreateMipMap();
	Draw::OutputRenderTextures();

	Blend::Blending(Blend::blendmode::off, Blend::blendop::add);
	Depth::Depth(Depth::depthmode::off);
	Rasterizer::Cull(Rasterizer::cullmode::off);

	Shaders::vShader(10);
	Shaders::pShader(100);
	context->Draw(6, 0);
}