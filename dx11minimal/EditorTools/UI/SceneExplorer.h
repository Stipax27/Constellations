#pragma once

#include "TreeViewModel.h"


class SceneExplorer : public ISingleton
{
public:
    void init(const std::vector<std::shared_ptr<Entity>>& rootObjects) {
        model.Rebuild(rootObjects);
        render();
    }

    void onToggleExpand(size_t index) {
        // 1. Обновляем модель
        model.ToggleExpand(index);

        // 2. Эффективно обновляем UI
        updateUI();
    }

private:
    TreeViewModel model;
    std::vector<UIButton*> uiButtons; // ваши кнопки

    void render() {
        // Первоначальная отрисовка
        uiButtons.clear();
        auto& items = model.GetFlatItems();
        for (size_t i = 0; i < items.size(); ++i) {
            auto btn = createButton(items[i]);
            btn->setPosition(calculatePosition(items[i].depth, i));
            uiButtons.push_back(btn);
        }
    }

    void updateUI() {
        auto& items = model.GetFlatItems();
        size_t currentItemCount = uiButtons.size();
        size_t newItemCount = items.size();

        if (newItemCount > currentItemCount) {
            // Добавляем новые кнопки
            for (size_t i = currentItemCount; i < newItemCount; ++i) {
                auto btn = createButton(items[i]);
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

    UIButton* createButton(const TreeItem& item) {
        // Создаем кнопку с отступом в зависимости от глубины
        auto btn = new UIButton();
        btn->setIndent(item.depth * INDENT_SIZE);
        btn->setText(item.object->name);
        btn->setExpandable(item.hasChildren);
        btn->setExpanded(item.isExpanded);
        return btn;
    }
};