#ifndef _DRAG_CONTROLLER_H_
#define _DRAG_CONTROLLER_H_

#include "../Engine/Camera/cameraclass.h"
#include "../Engine/Mouse/mouseclass.h"
#include "../Engine/Window/windowclass.h"

#include "../Engine/Physic/Collision/CollisionManagerClass.h"
#include "../Engine/ECS_Base/entityStorage.h"


#define SELECT_DISTANCE 10000.0f



class DragController : public ISingleton
{
public:
	void Initialize() override;
	void Shutdown();
	void Update();

private:
	CameraClass* camera;
	MouseClass* mouse;
	CollisionManagerClass* collisionManager;
	EntityStorage* entityStorage;

	Entity* dragEntity;
	Transform* dragTransform;

	float dragDistance;
	point3d dragOffset;

private:
	void ProcessPivotDrag();
	void DragByPivot();

	void ProcessSave();
};

#endif
