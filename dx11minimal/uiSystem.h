#ifndef _UI_SYSTEM_H_
#define _UI_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform2D.h"
#include "Rect.h"
#include "Button.h"
#include "TextLabel.h"
//#include "font.h"

#include "mouseclass.h"

/////////////
// GLOBALS //
/////////////
//const bool SHOW_UI_ANCHOR_POINTS = true;

///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////

//void preprocessFont()
//{
//	letter_width[0] = 14;
//	int letters_count = sizeof(letters) / sizeof(std::vector<float>*);
//
//	for (int letter = 1; letter < letters_count; letter++)
//	{
//		//min-max
//		float min_x = 1000000; float max_x = -100000;
//		for (int i = 0; i < letters[letter]->size(); i += 2)
//		{
//			float x = letters[letter]->at(i);
//			min_x = min(min_x, x);
//			max_x = max(max_x, x);
//		}
//
//		//offset x
//		for (int i = 0; i < letters[letter]->size(); i += 2)
//		{
//			letters[letter]->at(i) -= min_x;
//		}
//
//		//scale
//		float scale = .1;
//
//		for (int i = 0; i < letters[letter]->size(); i++)
//		{
//			letters[letter]->at(i) *= scale;
//		}
//
//
//		float width = max_x - min_x;
//
//		letter_width[letter] = width * scale;
//
//	}
//}
//
//float drawLetter(int letter, float x, float y, float scale, bool getSize = false)
//{
//	const int arcSteps = 4;
//	memset(ConstBuf::global, 0, sizeof(ConstBuf::global));
//	if (getSize)
//	{
//		return letter_width[letter] * scale;
//	}
//
//	if (letter == 0)
//	{
//		return letter_width[letter] * scale;
//	}
//
//	const std::vector<float>* pts = letters[letter];
//	int nPoints = (int)pts->size() / 2;
//	int instances = nPoints - 1;
//
//	for (int i = 0; i < nPoints - 1; i++)
//	{
//		float px0 = x + pts->at(i * 2) * scale;
//		float py0 = y + pts->at(i * 2 + 1) * scale;
//
//		float px1 = x + pts->at((i + 1) * 2) * scale;
//		float py1 = y + pts->at((i + 1) * 2 + 1) * scale;
//
//		ConstBuf::global[i * 2 + 0] = XMFLOAT4(px0, py0, 0.0f, 1.0f);
//		ConstBuf::global[i * 2 + 1] = XMFLOAT4(px1, py1, 0.0f, 1.0f);
//	}
//
//	ConstBuf::Update(5, ConstBuf::global);
//	ConstBuf::ConstToVertex(5);
//	ConstBuf::ConstToPixel(5);
//
//	ConstBuf::Update(0, ConstBuf::drawerV);
//	ConstBuf::ConstToVertex(0);
//	ConstBuf::Update(1, ConstBuf::drawerP);
//	ConstBuf::ConstToPixel(1);
//
//	context->DrawInstanced(6 + (arcSteps - 1) * 3, instances, 0, 0);
//
//	return letter_width[letter] * scale;
//}


class UISystem : public System
{
public:
	UISystem(MouseClass*, EntityStorage*);
	void Initialize();
	void Shutdown();

	void Update(vector<Entity*>&, float);

private:
	MouseClass* mouse;
	EntityStorage* entityStorage;

private:
	void DrawUiObject(Transform2D, int);
};

#endif