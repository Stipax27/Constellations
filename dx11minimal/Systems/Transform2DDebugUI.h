#ifndef _TRANSFORM2D_DEBUG_UI_H_
#define _TRANSFORM2D_DEBUG_UI_H_

class Entity;
class EntityStorage;

class Transform2DDebugUI
{
public:
	void Create(EntityStorage* storage, Entity* uiFolder);
	void UpdateToggle();

private:
	Entity* root = nullptr;
	bool visible = false;
	bool wasTogglePressed = false;
};

#endif
