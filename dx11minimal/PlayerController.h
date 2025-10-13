////////////////////////////////////////////////////////////////////////////////
// Filename: PlayerController.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PLAYERCONTROLLER_H_
#define _PLAYERCONTROLLER_H_

//////////////
// INCLUDES //
//////////////
#include "cameraclass.h"
#include "entity.h"

#include "Transform.cpp"
#include "PhysicBody.cpp"
#include "Constellation.cpp"

////////////////////////////////////////////////////////////////////////////////
// Class name: PlayerController
////////////////////////////////////////////////////////////////////////////////

class PlayerController
{
public:
	PlayerController();
	PlayerController(const PlayerController&);
	~PlayerController();

	void Initialize(Entity*, CameraClass*);
	void Shutdown();

	void ProcessInput();
	void ProcessCamera();

private:
	bool IsKeyPressed(const int);

private:
	Transform* playerTransform;
	PhysicBody* playerPhysicBody;
	Constellation* playerConstellation;

	CameraClass* camera;
};

#endif