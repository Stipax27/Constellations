#ifndef _UI_SYSTEM_
#define _UI_SYSTEM_

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
	UISystem(MouseClass* Mouse, EntityStorage* _EntityStorage)
	{
		mouse = Mouse;
		entityStorage = _EntityStorage;
	}


	void Initialize()
	{
	}


	void Shutdown()
	{
		if (mouse) {
			mouse = 0;
		}

		if (entityStorage) {
			entityStorage = 0;
		}
	}


	void Update(vector<Entity*>& entities, float deltaTime)
	{
		Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
		Rasterizer::Cull(Rasterizer::cullmode::off);
		Depth::Depth(Depth::depthmode::off);

		Shaders::gShader(0);

		size_t size = entities.size();
		for (int i = 0; i < size; i++) {
			Entity* entity = entities[i];
			if (IsEntityValid(entity) && entity->GetComponent<Transform2D>() != nullptr) {
				Transform2D transform2D = GetWorldTransform2D(entity);

				Rect* rect = entity->GetComponent<Rect>();
				if (rect != nullptr && rect->active && rect->opacity > 0.0f) {
					Shaders::pShader(13 + (int)rect->cornerType);

					ConstBuf::global[0].w = rect->cornerRadius;
					ConstBuf::global[3] = XMFLOAT4(rect->color.x, rect->color.y, rect->color.z, rect->opacity);

					DrawUiObject(transform2D);
				}

				Button* button = entity->GetComponent<Button>();
				if (button != nullptr && button->active) {

					//if (IsKeyPressed(VK_LBUTTON)) {
						point3d aspectCorrection = point3d();
						switch (transform2D.ratio)
						{
						case ScreenAspectRatio::XY:
							aspectCorrection = point3d(1, 1, 0);
							break;
						case ScreenAspectRatio::YX:
							aspectCorrection = point3d(ConstBuf::frame.aspect.x, ConstBuf::frame.aspect.y, 0);
							break;
						case ScreenAspectRatio::XX:
							aspectCorrection = point3d(1, ConstBuf::frame.aspect.y, 0);
							break;
						case ScreenAspectRatio::YY:
							aspectCorrection = point3d(ConstBuf::frame.aspect.x, 1, 0);
							break;
						}

						point3d realScale = transform2D.scale * aspectCorrection;
						point3d realPos = transform2D.position - transform2D.anchorPoint * realScale;
						point3d upVector = transform2D.GetUpVector();
						upVector.x *= aspectCorrection.y;
						upVector.y *= aspectCorrection.x;
						point3d rightVector = transform2D.GetRightVector() * aspectCorrection;
						rightVector.x *= aspectCorrection.y;
						rightVector.y *= aspectCorrection.x;

						point3d delta = mouse->pos - realPos;
						float projX = delta.dot(rightVector.normalized());
						float projY = delta.dot(upVector.normalized());

						if (abs(projX) <= realScale.x && abs(projY) <= realScale.y) {
							button->isClicked = true;
						}
						else {
							button->isClicked = false;
						}
					/*}
					else {
						button->isClicked = false;
					}*/

					if (button->isClicked) {
						button->color = point3d(1, 0, 0);

						Entity* _entity = entityStorage->CreateEntity();
						Transform2D* _transform2d = _entity->AddComponent<Transform2D>();
						_transform2d->position = mouse->pos;
						_transform2d->scale = point3d(0.02f, 0.02f, 0.0f);
						_transform2d->ratio = ScreenAspectRatio::YY;
						Rect* _rect = _entity->AddComponent<Rect>();
						_rect->color = point3d(0, 1, 0);
					}
					else {
						button->color = point3d(0.5f, 0.25f, 0.8f);
					}

					Shaders::pShader(13);
					ConstBuf::global[0].w = 0;
					ConstBuf::global[3] = XMFLOAT4(button->color.x, button->color.y, button->color.z, button->opacity);

					DrawUiObject(transform2D);
				}

				//TextLabel* textLabel = entity->GetComponent<TextLabel>();
				//if (textLabel != nullptr) {
				//	point3d pos = transform.position;
				//	const char* str = textLabel->text.c_str();

				//	Shaders::vShader(0);
				//	Shaders::pShader(0);

				//	ConstBuf::global[0] = XMFLOAT4(pos.x, pos.y, pos.z, textLabel->fontSize);

				//	preprocessFont();
				//	//scale = scale * window.width / 2560.;

				//	float tracking = 10;
				//	float interline = 40 * textLabel->fontSize;

				//	int letters_count = strlen(str);
				//	float base_x = pos.x;
				//	float base_y = pos.y;
				//	int i = 0;
				//	float maxStringWidth = 0;
				//	float stringWidth = 0;

				//	while (i < letters_count)
				//	{
				//		float offset = 0;

				//		/*if (centered)
				//		{
				//			int j = i;
				//			while (j < letters_count && str[j] != '\n')
				//			{
				//				offset += letter_width[str[j] - 32] * textLabel->fontSize + tracking;
				//				j++;
				//			}
				//			offset /= 2.;
				//		}*/

				//		while (i < letters_count && str[i] != '\n')
				//		{
				//			float sz = drawLetter(str[i] - 32, pos.x - offset, pos.y, textLabel->fontSize, textLabel->fontSize) + tracking;
				//			pos.x += sz;
				//			stringWidth += sz;
				//			i++;
				//		}

				//		maxStringWidth = max(maxStringWidth, stringWidth);

				//		i++;
				//		pos.x = base_x;
				//		pos.y += interline;
				//	}

				//	//return { maxStringWidth ,y - base_y,0 };
				//}
			}
		}
	}

private:
	MouseClass* mouse;
	EntityStorage* entityStorage;

private:
	void DrawUiObject(Transform2D transform2D) {
		Shaders::vShader(13);

		ConstBuf::global[0] = XMFLOAT4(transform2D.position.x, transform2D.position.y, transform2D.position.z, ConstBuf::global[0].w);
		ConstBuf::global[1] = XMFLOAT4(transform2D.scale.x, transform2D.scale.y, 0, 0);
		ConstBuf::global[2] = XMFLOAT4(transform2D.anchorPoint.x, transform2D.anchorPoint.y, transform2D.rotation, 0);

		switch (transform2D.ratio)
		{
		case ScreenAspectRatio::XY:
			ConstBuf::global[1].z = 1;
			ConstBuf::global[1].w = 1;
			break;
		case ScreenAspectRatio::YX:
			ConstBuf::global[1].z = ConstBuf::frame.aspect.x;
			ConstBuf::global[1].w = ConstBuf::frame.aspect.y;
			break;
		case ScreenAspectRatio::XX:
			ConstBuf::global[1].z = 1;
			ConstBuf::global[1].w = ConstBuf::frame.aspect.y;
			break;
		case ScreenAspectRatio::YY:
			ConstBuf::global[1].z = ConstBuf::frame.aspect.x;
			ConstBuf::global[1].w = 1;
			break;
		}

		ConstBuf::Update(5, ConstBuf::global);
		ConstBuf::ConstToVertex(5);
		ConstBuf::ConstToPixel(5);

		Draw::Drawer(1);
	}
};

#endif