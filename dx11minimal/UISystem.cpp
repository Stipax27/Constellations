#include "uiSystem.h"



UISystem::UISystem(MouseClass* Mouse, EntityStorage* _EntityStorage)
{
	mouse = Mouse;
	entityStorage = _EntityStorage;
}


void UISystem::Initialize()
{
}


void UISystem::Shutdown()
{
	if (mouse) {
		mouse = 0;
	}

	if (entityStorage) {
		entityStorage = 0;
	}
}


void UISystem::Update(vector<Entity*>& entities, float deltaTime)
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
				int pShader = rect->pShader == 13 ? 13 + (int)rect->cornerType : rect->pShader;
				Shaders::pShader(pShader);

				ConstBuf::global[0].w = rect->cornerRadius;
				ConstBuf::global[3] = XMFLOAT4(rect->color.x, rect->color.y, rect->color.z, rect->opacity);

				DrawUiObject(transform2D, rect->vShader);
			}

			Button* button = entity->GetComponent<Button>();
			if (button != nullptr && button->active) {

				if (mouse->IsLButtonDown()) {
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
					point3d rightVector = transform2D.GetRightVector();

					point3d delta = mouse->GetLButtonDownPosition() - realPos;
					delta.x *= ConstBuf::frame.aspect.y;
					float projX = delta.dot(rightVector) * ConstBuf::frame.aspect.x;
					float projY = delta.dot(upVector);

					if (abs(projX) <= realScale.x && abs(projY) <= realScale.y) {
						delta = mouse->pos - realPos;
						delta.x *= ConstBuf::frame.aspect.y;
						projX = delta.dot(rightVector) * ConstBuf::frame.aspect.x;
						projY = delta.dot(upVector);

						if (abs(projX) <= realScale.x && abs(projY) <= realScale.y) {
							button->isClicked = true;
						}
						else {
							button->isClicked = false;
						}
					}
				}
				else {
					button->isClicked = false;
				}

				point3d color = button->isClicked ? button->clickColor : button->color;

				//if (button->isClicked) {
				//	button->color = point3d(1, 0, 0);

				//	/*Entity* _entity = entityStorage->CreateEntity();
				//	Transform2D* _transform2d = _entity->AddComponent<Transform2D>();
				//	_transform2d->position = mouse->GetLButtonDownPosition();
				//	_transform2d->scale = point3d(0.02f, 0.02f, 0.0f);
				//	_transform2d->ratio = ScreenAspectRatio::YY;
				//	Rect* _rect = _entity->AddComponent<Rect>();
				//	_rect->color = point3d(0, 1, 0);*/
				//}
				//else {
				//	button->color = point3d(0.5f, 0.25f, 0.8f);
				//}

				int pShader = button->pShader == 13 ? 13 + (int)button->cornerType : button->pShader;
				Shaders::pShader(pShader);

				ConstBuf::global[0].w = button->cornerRadius;
				ConstBuf::global[3] = XMFLOAT4(color.x, color.y, color.z, button->opacity);

				DrawUiObject(transform2D, button->vShader);
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

			// DEBUG //

			if (SHOW_UI_ANCHOR_POINTS) {
				transform2D.scale = point3d(0.005f, 0.005f, 0);
				transform2D.anchorPoint = point3d();
				transform2D.rotation = 0;
				transform2D.ratio = ScreenAspectRatio::YY;

				point3d color = hslToRgb(point3d(sin(timer::currentTime / 1000.0f), 1.0f, 0.5f));

				ConstBuf::global[0].w = 1;
				ConstBuf::global[3] = XMFLOAT4(color.x, color.y, color.z, 1.0f);

				DrawUiObject(transform2D, 13);
			}

			///////////
		}
	}
}


void UISystem::DrawUiObject(Transform2D transform2D, int vShader) {
	Shaders::vShader(vShader);

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