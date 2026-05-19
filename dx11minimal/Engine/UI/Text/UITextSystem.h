#ifndef _UI_TEXT_SYSTEM_H_
#define _UI_TEXT_SYSTEM_H_

#include "../../ECS_Base/system.h"
#include "../../BasicComponents/Transform2D.h"
#include "TextLabel.h"

class UITextSystem : public System
{
public:
	UITextSystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);
};

#endif
