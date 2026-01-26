#ifndef _UI_SYSTEM_
#define _UI_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>
#include "system.h"
#include "Transform.h"
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
	UISystem(MouseClass* Mouse)
	{
		mouse = Mouse;
	}


	void Initialize()
	{
	}


	void Shutdown()
	{
		if (mouse) {
			mouse = 0;
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
			if (IsEntityValid(entity)) {
				Transform transform = GetWorldTransform(entity);

				Rect* rect = entity->GetComponent<Rect>();
				if (rect != nullptr && rect->active) {
						
					Button* button = entity->GetComponent<Button>();
					if (button != nullptr && button->active) {

						point3d halfSize = transform.scale / 2;
						point3d realPos = transform.position - rect->anchorPoint * halfSize;

						/*if (transform.position - (transform.scale / 2) * (rect->anchorPoint + 1) <= mouse->pos.x <= transform.position + (transform.scale / 2) * (rect->anchorPoint - 1)) {
							if (IsKeyPressed(VK_LBUTTON)) {
								rect->color = point3d(1, 0, 0);
							}
						}
						else {
							if (IsKeyPressed(VK_LBUTTON)) {
								rect->color = point3d(0.5f, 0.25f, 0.8f);
							}
						}*/

						if (mouse->pos.x <= realPos.x + transform.scale.x && mouse->pos.x >= realPos.x - transform.scale.x && mouse->pos.y <= realPos.y + transform.scale.y && mouse->pos.y >= realPos.y - transform.scale.y) {
							if (IsKeyPressed(VK_LBUTTON)) {
								rect->color = point3d(1, 0, 0);
							}
						}
						else {
							if (IsKeyPressed(VK_LBUTTON)) {
								rect->color = point3d(0.5f, 0.25f, 0.8f);
							}
						}
					}

					if (rect->opacity > 0.0f) {
						Shaders::vShader(13);
						Shaders::pShader(13 + (int)rect->cornerType);

						ConstBuf::global[0] = XMFLOAT4(transform.position.x, transform.position.y, transform.position.z, rect->cornerRadius);
						ConstBuf::global[1] = XMFLOAT4(transform.scale.x, transform.scale.y, 0, 0);
						ConstBuf::global[2] = XMFLOAT4(rect->anchorPoint.x, rect->anchorPoint.y, rect->rotation, 0);
						ConstBuf::global[3] = XMFLOAT4(rect->color.x, rect->color.y, rect->color.z, rect->opacity);

						switch (rect->ratio)
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
};

#endif