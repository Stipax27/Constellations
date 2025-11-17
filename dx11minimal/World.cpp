////////////////////////////////////////////////////////////////////////////////
// Filename: World.cpp
////////////////////////////////////////////////////////////////////////////////
#include "World.h"


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
	m_Camera->SetQuaternionRotation(0.0f, 1.0f, 0.0f, 0.0f);

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
}


Entity* World::CreateEntity(string Name, Entity* Parent)
{
	Entity* entity = new Entity;
	entity->name = Name;
	entity->parent = Parent;

	entities.push_back(entity);

	return entity;
}


void World::RemoveEntityByObject(Entity* object)
{
	for (int i = 0; i < entities.size(); i++) {
		Entity* entity = entities[i];
		if (entity == object) {
			entity->Destroy();
			entities.erase(entities.begin() + i);
			break;
		}
	}
}


void World::PreCalculations()
{
	InputAssembler::IA(InputAssembler::topology::triList);
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Depth::Depth(Depth::depthmode::off);
	Rasterizer::Cull(Rasterizer::cullmode::off);

	Textures::RenderTarget(3, 0);
	Shaders::vShader(10);
	Shaders::pShader(200);
	Draw::Clear({ 0.0f, 0.0f, 0.0f, 1.0f });
	context->Draw(6, 0);
	Textures::CreateMipMap();

	Textures::RenderTarget(4, 0);
	Shaders::pShader(201);
	Draw::Clear({ 0.0f, 0.0f, 0.0f, 1.0f });
	context->Draw(6, 0);
	Textures::CreateMipMap();

	Textures::RenderTarget(1, 0);
}


bool World::UpdatePhysic()
{
	bool result;
	size_t size = physicSystems.size();
	for (int i = 0; i < size; i++)
	{
		result = physicSystems[i]->Update(entities, 0.01f);
		if (!result)
		{
			return false;
		}
	}

	return true;
}


bool World::UpdateRender()
{
	bool result;

	// Updating time in const buffer
	ConstBuf::frame.time = XMFLOAT4{ (float)(timer::frameBeginTime * 0.01f), 0, 0, 0 };
	ConstBuf::UpdateFrame();
	ConstBuf::ConstToVertex(4);
	ConstBuf::ConstToPixel(4);

	if (firstFrame) {
		firstFrame = false;
		PreCalculations();
	}
	Textures::TextureToShader(3, 0);
	Textures::TextureToShader(4, 1);

	Textures::RenderTarget(1, 0);
	// Clear the buffers to begin the scene.
	Draw::Clear({ 0.0f, 0.0588f, 0.1176f, 1.0f });
	Draw::ClearDepth();

	size_t size = renderSystems.size();
	for (int i = 0; i < size; i++)
	{
		result = renderSystems[i]->Update(entities, timer::deltaTime);
		if (!result)
		{
			return false;
		}
	}

	Textures::CreateMipMap();
	Draw::OutputRenderTextures();

	Blend::Blending(Blend::blendmode::off, Blend::blendop::add);
	Depth::Depth(Depth::depthmode::off);
	Rasterizer::Cull(Rasterizer::cullmode::off);

	Shaders::vShader(10);
	Shaders::pShader(100);
	context->Draw(6, 0);

	return true;
}

void World::CleanMem()
{
	size_t size = entities.size();
	int i = 0;
	while (i < size)
	{
		Entity* entity = entities[i];
		if (entity == nullptr)
		{
			entities.erase(entities.begin() + i);
		}
		else
		{
			i++;
		}
	}
}