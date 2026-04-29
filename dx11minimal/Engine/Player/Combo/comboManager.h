#ifndef _COMBO_MANAGER_H_
#define _COMBO_MANAGER_H_

#include "combo.h"
#include "../../Lib/singleton.h"

/////////////
// GLOBALS //
/////////////

#define COMBO_BUFFER_DEBUG true

#define COMBO_BUFFER_LOCK_TIME 200
#define COMBO_PAUSE_TIME 500

/////////////
//  CLASS  //
/////////////

class ComboManager : public ISingleton
{
public:
	void Initialize();
	void Update();

	void SaveInput(ComboInputType input);

private:
	std::vector<ComboInputType> inputBuffer;
	std::vector<Combo> comboList;

	int maxComboLength;

	double lastInputTime;
	double bufferUnlockTime;

	EntityStorage* entityStorage;
	Entity* playerEntity;

#if COMBO_BUFFER_DEBUG
	Entity* bufferUi;
#endif

private:
	bool IsComboComplete(Combo& combo);

	void ClearInputBuffer();
	void LockInputBuffer(double time);
};

#endif