#include "Transform2DDebugUI.h"

#include "../Engine/ECS_Base/entity.h"
#include "../Engine/ECS_Base/entityStorage.h"

#include "../Engine/UI/Text/TextLabel.h"
#include "../Engine/UI/Rect.h"
#include "../Engine/BasicComponents/Transform2D.h"

#include "../Engine/Utils/utils.h"
#include "../Engine/Lib/input.h"

namespace {
	Transform2D* AddUiTransform(
		Entity* entity,
		const point3d& position,
		const point3d& scale,
		ScreenAspectRatio ratio = ScreenAspectRatio::XY,
		const point3d& anchorPoint = point3d(),
		const point3d& parentAnchor = point3d(),
		float rotation = 0.0f)
	{
		Transform2D* transform = entity->AddComponent<Transform2D>();
		transform->position = position;
		transform->scale = scale;
		transform->ratio = ratio;
		transform->anchorPoint = anchorPoint;
		transform->parentAnchor = parentAnchor;
		transform->rotation = rotation;
		return transform;
	}

	Rect* AddUiRect(Entity* entity, const point3d& color, float opacity = 1.0f, float cornerRadius = 0.0f)
	{
		Rect* rect = entity->AddComponent<Rect>();
		rect->color = color;
		rect->opacity = opacity;
		rect->cornerRadius = cornerRadius;
		rect->cornerType = CornerType::Strict;
		return rect;
	}

	TextLabel* AddUiText(
		Entity* entity,
		const wchar_t* text,
		const point3d& color = point3d(1.0f, 1.0f, 1.0f),
		float fontScale = 0.42f,
		int fontSizePx = 34)
	{
		TextLabel* textLabel = entity->AddComponent<TextLabel>();
		textLabel->textW = text;
		textLabel->fontFamilyW = L"Impact";
		textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
		textLabel->fontWeight = 900;
		textLabel->fontSizePx = fontSizePx;
		textLabel->fontScale = fontScale;
		textLabel->letterSpacingPx = 0.5f;
		textLabel->color = color;
		return textLabel;
	}

	Entity* CreateUiRect(
		EntityStorage* storage,
		const std::string& name,
		Entity* parent,
		const point3d& position,
		const point3d& scale,
		const point3d& color,
		float opacity = 1.0f,
		ScreenAspectRatio ratio = ScreenAspectRatio::XY,
		const point3d& anchorPoint = point3d(),
		const point3d& parentAnchor = point3d(),
		float rotation = 0.0f,
		float cornerRadius = 0.0f)
	{
		Entity* entity = storage->CreateEntity(name, parent);
		AddUiTransform(entity, position, scale, ratio, anchorPoint, parentAnchor, rotation);
		AddUiRect(entity, color, opacity, cornerRadius);
		return entity;
	}

	Entity* CreateUiText(
		EntityStorage* storage,
		const std::string& name,
		Entity* parent,
		const wchar_t* text,
		const point3d& position,
		const point3d& color = point3d(1.0f, 1.0f, 1.0f),
		const point3d& parentAnchor = point3d(-1.0f, 1.0f, 0.0f),
		float fontScale = 0.42f,
		int fontSizePx = 34)
	{
		Entity* entity = storage->CreateEntity(name, parent);
		AddUiTransform(entity, position, point3d(1.0f, 1.0f, 0.0f), ScreenAspectRatio::XY, point3d(), parentAnchor);
		AddUiText(entity, text, color, fontScale, fontSizePx);
		return entity;
	}

	Entity* CreateParentAnchorMarker(EntityStorage* storage, const std::string& name, Entity* parent, const point3d& parentAnchor)
	{
		return CreateUiRect(
			storage,
			name,
			parent,
			point3d(),
			point3d(0.05f, 0.05f, 0.0f),
			point3d(1.0f, 0.86f, 0.15f),
			1.0f,
			ScreenAspectRatio::XX,
			point3d(),
			parentAnchor,
			0.0f,
			1.0f);
	}
}

void Transform2DDebugUI::Create(EntityStorage* storage, Entity* uiFolder)
{
	root = storage->CreateEntity("Transform2D Debug UI", uiFolder);
	AddUiTransform(root, point3d(), point3d(1.0f, 1.0f, 0.0f));
	root->SetActive(false);
	visible = false;

	CreateUiRect(storage, "Debug fullscreen background", root, point3d(), point3d(1.0f, 1.0f, 0.0f), point3d(0.02f, 0.03f, 0.05f), 0.82f);
	CreateUiText(storage, "Debug title", root, L"Transform2D anchors/pivots demo - press U to hide/show", point3d(0.08f, -0.08f, 0.0f), point3d(1.0f, 0.95f, 0.72f), point3d(-1.0f, 1.0f, 0.0f), 0.58f, 42);
	CreateUiText(storage, "Debug legend", root, L"Yellow squares = parentAnchor. Blinking dots = object pivot / anchorPoint.", point3d(0.08f, -0.19f, 0.0f), point3d(0.72f, 0.9f, 1.0f), point3d(-1.0f, 1.0f, 0.0f), 0.46f, 36);

	Entity* centerPanel = CreateUiRect(storage, "Center anchored parent", root, point3d(-0.72f, 0.44f, 0.0f), point3d(0.20f, 0.12f, 0.0f), point3d(0.12f, 0.33f, 0.75f), 0.82f, ScreenAspectRatio::XY, point3d(), point3d(), 0.0f, 0.08f);
	CreateUiText(storage, "Center panel label", centerPanel, L"center / center", point3d(0.06f, -0.13f, 0.0f), point3d(1.0f, 1.0f, 1.0f), point3d(-1.0f, 1.0f, 0.0f), 0.34f, 28);
	CreateParentAnchorMarker(storage, "Center parent anchor marker", centerPanel, point3d());
	CreateUiRect(storage, "Center child", centerPanel, point3d(), point3d(0.35f, 0.35f, 0.0f), point3d(0.2f, 0.95f, 0.55f), 0.95f, ScreenAspectRatio::XY, point3d(), point3d(), 0.0f, 0.18f);

	Entity* cornerPanel = CreateUiRect(storage, "Corner anchored parent", root, point3d(0.46f, 0.58f, 0.0f), point3d(0.20f, 0.12f, 0.0f), point3d(0.46f, 0.18f, 0.68f), 0.82f, ScreenAspectRatio::XY, point3d(-1.0f, 1.0f, 0.0f), point3d(), 0.0f, 0.08f);
	CreateUiText(storage, "Corner panel label", cornerPanel, L"pivot top-left", point3d(0.06f, -0.13f, 0.0f), point3d(1.0f, 1.0f, 1.0f), point3d(-1.0f, 1.0f, 0.0f), 0.34f, 28);
	CreateParentAnchorMarker(storage, "Top left parent anchor marker", cornerPanel, point3d(-1.0f, 1.0f, 0.0f));
	CreateParentAnchorMarker(storage, "Bottom right parent anchor marker", cornerPanel, point3d(1.0f, -1.0f, 0.0f));
	CreateUiRect(storage, "Top left child", cornerPanel, point3d(0.04f, -0.04f, 0.0f), point3d(0.30f, 0.30f, 0.0f), point3d(1.0f, 0.58f, 0.18f), 0.95f, ScreenAspectRatio::XY, point3d(-1.0f, 1.0f, 0.0f), point3d(-1.0f, 1.0f, 0.0f), 0.0f, 0.12f);
	CreateUiRect(storage, "Bottom right child", cornerPanel, point3d(-0.04f, 0.04f, 0.0f), point3d(0.26f, 0.26f, 0.0f), point3d(0.95f, 0.28f, 0.38f), 0.95f, ScreenAspectRatio::XY, point3d(1.0f, -1.0f, 0.0f), point3d(1.0f, -1.0f, 0.0f), 0.0f, 0.12f);

	Entity* rotatedPanel = CreateUiRect(storage, "Rotated parent", root, point3d(-0.72f, -0.54f, 0.0f), point3d(0.21f, 0.11f, 0.0f), point3d(0.08f, 0.50f, 0.47f), 0.82f, ScreenAspectRatio::XY, point3d(), point3d(), 0.55f, 0.08f);
	CreateUiText(storage, "Rotated panel label", rotatedPanel, L"parent rotation", point3d(0.06f, -0.12f, 0.0f), point3d(1.0f, 1.0f, 1.0f), point3d(-1.0f, 1.0f, 0.0f), 0.34f, 28);
	CreateParentAnchorMarker(storage, "Right edge parent anchor marker", rotatedPanel, point3d(1.0f, 0.0f, 0.0f));
	CreateUiRect(storage, "Rotated right edge child", rotatedPanel, point3d(0.08f, 0.0f, 0.0f), point3d(0.25f, 0.45f, 0.0f), point3d(0.38f, 1.0f, 0.38f), 0.95f, ScreenAspectRatio::XY, point3d(), point3d(1.0f, 0.0f, 0.0f), 0.0f, 0.12f);

	Entity* stretchPanel = CreateUiRect(storage, "Aspect ratio parent", root, point3d(0.56f, -0.54f, 0.0f), point3d(0.23f, 0.11f, 0.0f), point3d(0.48f, 0.42f, 0.13f), 0.82f, ScreenAspectRatio::XY, point3d(), point3d(), 0.0f, 0.08f);
	CreateUiText(storage, "Aspect ratio label", stretchPanel, L"ratio XX / YY", point3d(0.06f, -0.12f, 0.0f), point3d(1.0f, 1.0f, 1.0f), point3d(-1.0f, 1.0f, 0.0f), 0.34f, 28);
	CreateParentAnchorMarker(storage, "Left ratio anchor marker", stretchPanel, point3d(-0.55f, 0.0f, 0.0f));
	CreateParentAnchorMarker(storage, "Right ratio anchor marker", stretchPanel, point3d(0.55f, 0.0f, 0.0f));
	CreateUiRect(storage, "YY child", stretchPanel, point3d(), point3d(0.24f, 0.24f, 0.0f), point3d(0.35f, 0.75f, 1.0f), 0.95f, ScreenAspectRatio::YY, point3d(), point3d(-0.55f, 0.0f, 0.0f), 0.0f, 0.15f);
	CreateUiRect(storage, "XX child", stretchPanel, point3d(), point3d(0.24f, 0.24f, 0.0f), point3d(1.0f, 0.45f, 0.88f), 0.95f, ScreenAspectRatio::XX, point3d(), point3d(0.55f, 0.0f, 0.0f), 0.0f, 0.15f);

	Entity* deepRoot = CreateUiRect(storage, "Deep nesting root", root, point3d(0.0f, -0.06f, 0.0f), point3d(0.40f, 0.34f, 0.0f), point3d(0.12f, 0.12f, 0.16f), 0.82f, ScreenAspectRatio::XY, point3d(), point3d(), 0.0f, 0.06f);
	CreateUiText(storage, "Deep nesting label", deepRoot, L"5 nested transforms", point3d(0.05f, -0.12f, 0.0f), point3d(1.0f, 0.95f, 0.72f), point3d(-1.0f, 1.0f, 0.0f), 0.50f, 38);

	Entity* level1 = CreateUiRect(storage, "Deep level 1", deepRoot, point3d(0.0f, 0.0f, 0.0f), point3d(0.76f, 0.76f, 0.0f), point3d(0.82f, 0.25f, 0.20f), 0.72f, ScreenAspectRatio::XY, point3d(), point3d(), 0.22f, 0.04f);
	CreateParentAnchorMarker(storage, "Deep level 1 anchor", deepRoot, point3d());
	CreateUiText(storage, "Deep level 1 label", deepRoot, L"1 pos(0,0,0)", point3d(0.03f, -0.07f, 0.0f), point3d(1.0f, 1.0f, 1.0f), point3d(), 0.30f, 26);

	Entity* level2 = CreateUiRect(storage, "Deep level 2", level1, point3d(0.08f, 0.0f, 0.0f), point3d(0.76f, 0.76f, 0.0f), point3d(0.95f, 0.62f, 0.18f), 0.72f, ScreenAspectRatio::XY, point3d(), point3d(), 0.22f, 0.04f);
	CreateParentAnchorMarker(storage, "Deep level 2 anchor", level1, point3d());
	CreateUiText(storage, "Deep level 2 label", level1, L"2 pos(0.08,0,0)", point3d(0.11f, -0.07f, 0.0f), point3d(1.0f, 1.0f, 1.0f), point3d(), 0.30f, 26);

	Entity* level3 = CreateUiRect(storage, "Deep level 3", level2, point3d(0.08f, 0.0f, 0.0f), point3d(0.76f, 0.76f, 0.0f), point3d(0.15f, 0.70f, 0.95f), 0.72f, ScreenAspectRatio::XY, point3d(), point3d(), 0.22f, 0.04f);
	CreateParentAnchorMarker(storage, "Deep level 3 anchor", level2, point3d());
	CreateUiText(storage, "Deep level 3 label", level2, L"3 pos(0.08,0,0)", point3d(0.11f, -0.07f, 0.0f), point3d(1.0f, 1.0f, 1.0f), point3d(), 0.30f, 26);

	Entity* level4 = CreateUiRect(storage, "Deep level 4", level3, point3d(0.08f, 0.0f, 0.0f), point3d(0.76f, 0.76f, 0.0f), point3d(0.72f, 0.34f, 0.95f), 0.72f, ScreenAspectRatio::XY, point3d(), point3d(), 0.22f, 0.04f);
	CreateParentAnchorMarker(storage, "Deep level 4 anchor", level3, point3d());
	CreateUiText(storage, "Deep level 4 label", level3, L"4 pos(0.08,0,0)", point3d(0.11f, -0.07f, 0.0f), point3d(1.0f, 1.0f, 1.0f), point3d(), 0.30f, 26);

	Entity* level5 = CreateUiRect(storage, "Deep level 5", level4, point3d(0.08f, 0.0f, 0.0f), point3d(0.76f, 0.76f, 0.0f), point3d(0.40f, 1.0f, 0.45f), 0.82f, ScreenAspectRatio::XY, point3d(), point3d(), 0.22f, 0.04f);
	CreateParentAnchorMarker(storage, "Deep level 5 anchor", level4, point3d());
	CreateUiText(storage, "Deep level 5 label", level4, L"5 pos(0.08,0,0)", point3d(0.11f, -0.07f, 0.0f), point3d(1.0f, 1.0f, 1.0f), point3d(), 0.30f, 26);

	CreateUiRect(storage, "Deep final marker", level5, point3d(), point3d(0.18f, 0.18f, 0.0f), point3d(1.0f, 1.0f, 1.0f), 1.0f, ScreenAspectRatio::XX, point3d(), point3d(), 0.0f, 1.0f);
}

void Transform2DDebugUI::UpdateToggle()
{
	const bool isTogglePressed = input::IsKeyPressed('U');
	if (!isTogglePressed)
	{
		wasTogglePressed = false;
		return;
	}

	if (wasTogglePressed)
	{
		return;
	}

	wasTogglePressed = true;
	visible = !visible;

	if (root)
	{
		root->SetActive(visible);
	}
}
