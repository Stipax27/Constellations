#ifndef _EDITOR_UI_H_
#define _EDITOR_UI_H_

#include "EditCameraController.h"
#include "DragController.h"


class EditorUI : public ISingleton
{
public:
	void Initialize() override;
	void Shutdown();
	void Update();

private:
	DragController* dragController;
};

#endif
