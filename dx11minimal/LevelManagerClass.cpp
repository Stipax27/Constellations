#include "LevelManagerClass.h"

LevelManagerClass::LevelManagerClass()
{
	m_World = 0;
}

LevelManagerClass::LevelManagerClass(const LevelManagerClass& other)
{
}


LevelManagerClass::~LevelManagerClass()
{
}


bool LevelManagerClass::Initialize()
{
	char bitmapFilename[128];
	char modelFilename[128];
	char textureFilename[128];
	bool result;

	//////////////////////////////////////////////////////////////////////////////////////////////
	// WORLD CREATING START //
	//////////////////////////////////////////////////////////////////////////////////////////////

	m_World = new World;
	result = m_World->Initialize();
	if (!result)
	{
		return false;
	}

	Sprite* sprite;
	Transform* transform;
	PhysicBody* physicBody;
	//SphereCollider* sphereCollider;

	Entity* star = m_World->CreateEntity();
	transform = star->AddComponent<Transform>();
	transform->position = point3d(0.0f, 3.0f, 0.0f);
	physicBody = star->AddComponent<PhysicBody>();
	physicBody->velocity = point3d(0.0f, 0.0f, 0.0f);
	//star->AddComponent<SphereCollider>();
	sprite = star->AddComponent<Sprite>();
	sprite->vShader = 1;
	sprite->pShader = 1;

	m_World->AddPhysicSystem<PhysicSystem>();
	m_World->AddPhysicSystem<CollisionSystem>();
	m_World->AddRenderSystem<RenderSystem>(m_World->m_Camera);

	//////////////////////////////////////////////////////////////////////////////////////////////
	// WORLD CREATING END //
	//////////////////////////////////////////////////////////////////////////////////////////////


	return true;
}


void LevelManagerClass::Shutdown()
{
	if (m_World)
	{
		m_World->Shutdown();
		delete m_World;
		m_World = 0;
	}

	return;
}


bool LevelManagerClass::Frame()
{
	bool result;

	result = m_World->UpdatePhysic();
	if (!result)
	{
		return false;
	}

	result = m_World->UpdateRender();
	if (!result)
	{
		return false;
	}

	return true;
}