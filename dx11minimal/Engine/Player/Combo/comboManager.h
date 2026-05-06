#ifndef _COMBO_MANAGER_H_
#define _COMBO_MANAGER_H_

#include "combo.h"
#include "../../Lib/singleton.h"
#include "../PlayerAbilities.h"

/////////////
// GLOBALS //
/////////////

#define COMBO_BUFFER_DEBUG true

#define COMBO_BUFFER_LOCK_TIME 100
#define COMBO_PAUSE_TIME 750
#define COMBO_HELD_INPUT_START 200

/////////////
//  CLASS  //
/////////////

class ComboManager : public ISingleton
{
public:
	void Initialize();
	void Update();

	void SaveInput(ComboInputType input);
	void StartHeldInput(ComboInputType input);
	void ClearInputBuffer();

private:
	std::vector<ComboInputType> inputBuffer;
	std::vector<Combo> comboList;

	int maxComboLength;

	double lastInputTime;
	double bufferUnlockTime;

	ComboInputType heldInput;
	double heldInputTime;

	EntityStorage* entityStorage;
	Entity* playerEntity;
	PlayerAbilities* abilities;

#if COMBO_BUFFER_DEBUG
	Entity* bufferUi;
#endif

private:
	bool IsComboComplete(Combo& combo);

	void LockInputBuffer(double time);
	ComboInputType GetCITforCurrWeapon();

#if COMBO_BUFFER_DEBUG
	void PrintComboInput(const ComboInputType input);
#endif
};

#endif