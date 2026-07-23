#include "SceneExplorer.h"


void SceneExplorer::Initialize()
{
    entityStorage = Singleton::GetInstance<EntityStorage>();

    CreateExplorerWindow();

	model.Rebuild(rootObjects);
	Render();
}


void SceneExplorer::OnToggleExpand(size_t index) {
    // 1. Обновляем модель
    model.ToggleExpand(index);

    // 2. Эффективно обновляем UI
    UpdateUI();
}


void SceneExplorer::Render() {
    // Первоначальная отрисовка
    uiButtons.clear();
    auto& items = model.GetFlatItems();
    for (size_t i = 0; i < items.size(); ++i) {
        auto btn = createButton(items[i]);
        btn->setPosition(calculatePosition(items[i].depth, i));
        uiButtons.push_back(btn);
    }
}


void SceneExplorer::UpdateUI() {
    auto& items = model.GetFlatItems();
    size_t currentItemCount = uiButtons.size();
    size_t newItemCount = items.size();

    if (newItemCount > currentItemCount) {
        // Добавляем новые кнопки
        for (size_t i = currentItemCount; i < newItemCount; ++i) {
            auto btn = CreateButton(items[i]);
            uiButtons.push_back(btn);
        }
    }
    else if (newItemCount < currentItemCount) {
        // Удаляем лишние кнопки
        for (size_t i = newItemCount; i < currentItemCount; ++i) {
            removeButton(uiButtons[i]);
        }
        uiButtons.resize(newItemCount);
    }

    // Обновляем позиции всех кнопок (смещаем)
    for (size_t i = 0; i < uiButtons.size(); ++i) {
        uiButtons[i]->setPosition(calculatePosition(items[i].depth, i));
        uiButtons[i]->updateText(items[i].object->name);
        uiButtons[i]->setExpandable(items[i].hasChildren);
    }
}


Entity* SceneExplorer::CreateButton(const TreeItem& item) {
    // Button

    Entity* itemEntity = entityStorage->CreateEntity("Item", explorerWindow);

    Transform2D* transform2D = itemEntity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(0, 1, 0);
    transform2D->position = point3d(0, 1.0f - (EXPLORER_ITEM_HEIGHT * 2 * pos + EXPLORER_ITEM_OFFSET * (pos + 1)), 0);
    transform2D->scale = point3d(0.95f, EXPLORER_ITEM_HEIGHT, 0);

    Button* button = itemEntity->AddComponent<Button>();
    button->color = point3d(0.4f, 0.4f, 0.4f);
    button->clickColor = point3d(0.5f, 0.5f, 0.5f);

    TextLabel* textLabel = itemEntity->AddComponent<TextLabel>();
    textLabel->textW = string_to_wstring(item.entity->name);
    textLabel->fontFamilyW = L"Impact";
    textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
    textLabel->fontWeight = 500;
    textLabel->fontSizePx = 40;
    textLabel->fontScale = 0.6f;
    textLabel->letterSpacingPx = 1.0f;
    textLabel->centered = true;

    // Arrow holder

    Entity* arrowHolderEntity = entityStorage->CreateEntity("ArrowHolder", itemEntity);

    transform2D = arrowHolderEntity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(-1, 0, 0);
    transform2D->position = point3d(-0.98f, 0, 0);
    transform2D->scale = point3d(0.9f, 0.9f, 0);
    transform2D->ratio = ScreenAspectRatio::YY;

    // Arrow

    Entity* arrowEntity = entityStorage->CreateEntity("Arrow", arrowHolderEntity);

    transform2D = arrowEntity->AddComponent<Transform2D>();

    ImageLabel* imageLabel = arrowEntity->AddComponent<ImageLabel>();
    imageLabel->textureName = "itemArrow";

    itemButtons.push_back({ itemEntity, item });

    // Создаем кнопку с отступом в зависимости от глубины
    auto btn = new UIButton();
    btn->setIndent(item.depth * INDENT_SIZE);
    btn->setText(item.object->name);
    btn->setExpandable(item.hasChildren);
    btn->setExpanded(item.isExpanded);
    return btn;
}


void SceneExplorer::CreateExplorerWindow()
{
    explorerWindow = entityStorage->CreateEntity("Explorer");

    Transform2D* transform2D = explorerWindow->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(-1, 0, 0);
    transform2D->position = point3d(-1, 0, 0);
    transform2D->scale = point3d(EXPLORER_WIDTH, 1, 0);

    Rect* rect = explorerWindow->AddComponent<Rect>();
    rect->color = point3d(0.25f, 0.25f, 0.25f);
    rect->opacity = 0.75f;
}