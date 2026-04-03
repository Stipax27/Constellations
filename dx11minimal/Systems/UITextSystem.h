#ifndef _UI_TEXT_SYSTEM_H_
#define _UI_TEXT_SYSTEM_H_

#include "../system.h"
#include "../Engine/BasicComponents/Transform2D.h"
#include "../Components/TextLabel.h"

class UITextSystem : public System
{
public:
	UITextSystem();
	void Initialize();
	void Shutdown();

	void Update(EntityStorage&, float);
};

#endif
