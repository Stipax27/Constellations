#include "SceneExplorer.h"

using namespace std;


void SceneExplorer::Initialize()
{
    entityStorage = Singleton::GetInstance<EntityStorage>();

    CreateExplorerWindow();

	model.Rebuild(GetRootEntities());
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
        auto btn = CreateButton(items[i]);
        SetPosition(btn, CalculatePosition(i, items[i].depth));
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
            Entity* btn = CreateButton(items[i]);
            uiButtons.push_back(btn);
        }
    }
    else if (newItemCount < currentItemCount) {
        // Удаляем лишние кнопки
        for (size_t i = newItemCount; i < currentItemCount; ++i) {
            RemoveButton(i);
        }
        uiButtons.resize(newItemCount);
    }

    // Обновляем позиции всех кнопок (смещаем)
    for (size_t i = 0; i < uiButtons.size(); ++i) {
        SetPosition(uiButtons[i], CalculatePosition(i, items[i].depth));
        UpdateText(items[i]);
        SetExpandable(items[i]);
    }
}


void SceneExplorer::RemoveButton(size_t index) {
    Entity* button = uiButtons[index];
    button->Destroy();
    uiButtons.erase(uiButtons.begin() + index);
}


Entity* SceneExplorer::CreateButton(const TreeItem& item) {
    // Button

    Entity* itemEntity = entityStorage->CreateEntity("Item", explorerWindow);

    Transform2D* transform2D = itemEntity->AddComponent<Transform2D>();
    transform2D->anchorPoint = point3d(0, 1, 0);
    //transform2D->position = CalculatePosition(i, item.depth);
    transform2D->scale = point3d(0.95f, EXPLORER_ITEM_HEIGHT, 0);

    Button* button = itemEntity->AddComponent<Button>();
    button->color = point3d(0.4f, 0.4f, 0.4f);
    button->clickColor = point3d(0.5f, 0.5f, 0.5f);

    TextLabel* textLabel = itemEntity->AddComponent<TextLabel>();
    textLabel->textW = string_to_wstring(item.object->name);
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

    return itemEntity;
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


float SceneExplorer::CalculatePosition(size_t index, int depth) {
    return 1.0f - (EXPLORER_ITEM_HEIGHT * 2 * index + EXPLORER_ITEM_OFFSET * (index + 1));
}

void SceneExplorer::SetPosition(Entity* button, float position) {
    button->GetComponent<Transform2D>()->position = point3d(0, position, 0);
}

void SceneExplorer::UpdateText(const TreeItem& item) {
    item.object->GetComponent<TextLabel>()->textW = string_to_wstring(item.object->name);
}

void SceneExplorer::SetExpandable(const TreeItem& item) {
    Entity* arrowHolder = item.object->GetChildByName("ArrowHolder");
    arrowHolder->SetActive(item.hasChildren);
}


vector<Entity*> SceneExplorer::GetRootEntities() {
    vector<Entity*> list;

    EntityStorage* entityStorage = Singleton::GetInstance<EntityStorage>();
    for (Entity* entity : entityStorage->entities) {
        if (entity->GetParent() == nullptr) {
            list.push_back(entity);
        }
    }

    return list;
}