#include "LevelManagerClass.h"

LevelManagerClass::LevelManagerClass()
{
	m_World = 0;
	m_Direct3D = 0;
	m_Bitmap = 0;
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

	Mesh* mesh;
	Transform* transform;
	PhysicBody* physicBody;
	SphereCollider* sphereCollider;

	Entity* cube = m_World->CreateEntity();
	transform = cube->AddComponent<Transform>();
	transform->position = point3d(0.0f, 3.0f, 0.0f);
	physicBody = cube->AddComponent<PhysicBody>();
	physicBody->velocity = point3d(1.0f, 3.0f, 2.0f);
	//physicBody->angVelocity = point3d(5.0f, 10.0f, 3.0f);
	cube->AddComponent<SphereCollider>();
	mesh = cube->AddComponent<Mesh>();

	strcpy_s(modelFilename, "Data/Models/Cube.txt");
	strcpy_s(textureFilename, "Data/Textures/seafloor.tga");

	result = mesh->model.Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	Entity* cube1 = m_World->CreateEntity();
	transform = cube1->AddComponent<Transform>();
	transform->position = point3d(0.0f, 2.0f, 0.0f);
	transform->scale = point3d(0.5f, 0.5f, 0.5f);
	physicBody = cube1->AddComponent<PhysicBody>();
	physicBody->velocity = point3d(-1.0f, 2.0f, 1.0f);
	//physicBody->angVelocity = point3d(5.0f, 10.0f, 3.0f);
	sphereCollider = cube1->AddComponent<SphereCollider>();
	sphereCollider->radius = 0.5f;
	mesh = cube1->AddComponent<Mesh>();

	result = mesh->model.Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	Entity* cube2 = m_World->CreateEntity();
	transform = cube2->AddComponent<Transform>();
	transform->position = point3d(1.0f, 1.0f, 0.0f);
	transform->scale = point3d(0.8f, 0.8f, 0.8f);
	physicBody = cube2->AddComponent<PhysicBody>();
	physicBody->velocity = point3d(3.0f, 2.0f, 0.2f);
	//physicBody->angVelocity = point3d(5.0f, 10.0f, 3.0f);
	sphereCollider = cube2->AddComponent<SphereCollider>();
	sphereCollider->radius = 0.8f;
	mesh = cube2->AddComponent<Mesh>();

	result = mesh->model.Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	Entity* cube3 = m_World->CreateEntity();
	transform = cube3->AddComponent<Transform>();
	transform->position = point3d(3.0f, 4.0f, 0.0f);
	transform->scale = point3d(0.2f, 0.2f, 0.2f);
	physicBody = cube3->AddComponent<PhysicBody>();
	physicBody->velocity = point3d(-1.0f, 0.0f, -0.5f);
	//physicBody->angVelocity = point3d(5.0f, 10.0f, 3.0f);
	sphereCollider = cube3->AddComponent<SphereCollider>();
	sphereCollider->radius = 0.2f;
	mesh = cube3->AddComponent<Mesh>();

	result = mesh->model.Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	Entity* plane = m_World->CreateEntity();
	transform = plane->AddComponent<Transform>();
	transform->position = point3d(0.0f, -2.0f, 0.0f);
	transform->scale = point3d(10.0f, 0.001f, 10.0f);
	plane->AddComponent<PlaneCollider>();
	mesh = plane->AddComponent<Mesh>();

	result = mesh->model.Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFilename, textureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}


	m_World->AddPhysicSystem<PhysicSystem>();
	m_World->AddPhysicSystem<CollisionSystem>();
	m_World->AddRenderSystem<RenderSystem>(m_Direct3D, m_World->m_Camera);

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

	// Release the bitmap object.
	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
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