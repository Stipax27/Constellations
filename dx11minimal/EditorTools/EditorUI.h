#ifndef _EDITOR_UI_H_
#define _EDITOR_UI_H_

#include "DragController.h"


#define EXPLORER_WIDTH 0.2f


class EditorUI : public ISingleton
{
public:
	void Initialize() override;
	void Shutdown();
	void Update();

private:
	DragController* dragController;
	EntityStorage* entityStorage;

private:
	void InitExplorer();
};

#endif
