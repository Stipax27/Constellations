#ifndef _TREE_VIEW_MODEL_H_
#define _TREE_VIEW_MODEL_H_

#include "../../ecsHeaders.h"

#define EXPLORER_WIDTH 0.2f
#define EXPLORER_ITEM_HEIGHT 0.025f
#define EXPLORER_ITEM_OFFSET 0.01f


struct TreeItem {
	std::shared_ptr<Entity> object = nullptr;    // ссылка на объект сцены
	int depth = 0;                               // уровень вложенности
	bool hasChildren = false;                    // есть ли дети
	bool isExpanded = false;                     // раскрыт ли
	size_t indexInFlatList = 0;                  // позиция в плоском списке
};


class TreeViewModel
{
public:
    // Обновляет плоский список на основе корневых объектов
    void Rebuild(const std::vector<std::shared_ptr<Entity>>& rootObjects);

    // Получить плоский список для отрисовки
    const std::vector<TreeItem>& GetFlatItems() const;

    // Переключить раскрытие узла
    void ToggleExpand(size_t flatIndex);

private:
    std::vector<TreeItem> flatItems;

    // Рекурсивное "разворачивание" дерева в плоский список
    void Flatten(Entity* obj, int depth);

    // Эффективное обновление после переключения
    void RebuildFrom(size_t startIndex);
};

#endif