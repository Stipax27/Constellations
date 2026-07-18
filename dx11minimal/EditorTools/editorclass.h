#ifndef _EDITOR_CLASS_H_
#define _EDITOR_CLASS_H_

#include "EditCameraController.h"
#include "DragController.h"


class EditorClass : public ISingleton
{
public:
	void Initialize() override;
	void Shutdown();
	void Update();

private:
	EditCameraController* editCameraController;
	DragController* dragController;
};

#endif
