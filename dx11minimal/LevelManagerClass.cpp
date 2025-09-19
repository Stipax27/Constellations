#include "LevelManagerClass.h"

LevelManagerClass::LevelManagerClass()
{
	window = 0;
	mouse = 0;

	m_World = 0;
}

LevelManagerClass::LevelManagerClass(const LevelManagerClass& other)
{
}


LevelManagerClass::~LevelManagerClass()
{
}


void LevelManagerClass::InitWindow()
{
	if (window == 0) {
		window = new WindowStruct;
	}
}


bool LevelManagerClass::Initialize()
{
	char bitmapFilename[128];
	char modelFilename[128];
	char textureFilename[128];
	bool result;

	InitWindow();

	m_World = new World;
	result = m_World->Initialize(window->iaspect);
	if (!result)
	{
		return false;
	}

	mouse = new MouseStruct;
	mouse->Initialize(window, m_World->m_Camera);

	Dx11Init(window->hWnd, window->width, window->height);

	// window params into const buffer
	ConstBuf::frame.aspect = XMFLOAT4(window->aspect, window->iaspect, float(window->width), float(window->height));

	//////////////////////////////////////////////////////////////////////////////////////////////
	// WORLD CREATING START //
	//////////////////////////////////////////////////////////////////////////////////////////////

	Sprite* sprite;
	Transform* transform;
	PhysicBody* physicBody;
	//SphereCollider* sphereCollider;

	Entity* star = m_World->CreateEntity();
	transform = star->AddComponent<Transform>();
	transform->position = point3d(0.0f, 0.0f, 0.0f);
	transform->scale = point3d(5, 5, 5);
	//physicBody = star->AddComponent<PhysicBody>();
	//physicBody->velocity = point3d(0.0f, 0.0f, 0.0f);
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

	if (mouse)
	{
		delete mouse;
		mouse = 0;
	}

	if (window)
	{
		delete window;
		window = 0;
	}

	return;
}


bool LevelManagerClass::Frame()
{
	bool result;

	mouse->Update();

	XMFLOAT4 i = XMFLOAT4{ float(window->aspect), float(window->iaspect), float(window->width), float(window->height) };
	ConstBuf::frame.aspect = i;
	float a = ConstBuf::frame.aspect.x;

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