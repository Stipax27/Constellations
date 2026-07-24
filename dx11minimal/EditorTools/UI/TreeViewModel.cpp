#include "TreeViewModel.h"

using namespace std;


void TreeViewModel::Rebuild(const vector<Entity*>& rootObjects) {
    flatItems.clear();
    for (Entity* root : rootObjects) {
        Flatten(root, 0);
    }
}

// Получить плоский список для отрисовки
const vector<TreeItem>& TreeViewModel::GetFlatItems() const { return flatItems; }

// Переключить раскрытие узла
void TreeViewModel::ToggleExpand(size_t flatIndex) {
    if (flatIndex >= flatItems.size()) return;

    auto& item = flatItems[flatIndex];
    if (!item.hasChildren) return;

    // Переключаем состояние в самом объекте
    item.isExpanded = !item.isExpanded;

    // Перестраиваем плоский список (но эффективно, а не с нуля!)
    RebuildFrom(flatIndex);
}


// Рекурсивное "разворачивание" дерева в плоский список
void TreeViewModel::Flatten(Entity* obj, int depth) {
    vector<Entity*> children = obj->GetChildren();

    TreeItem item;
    item.object = shared_ptr<Entity>(obj); // или shared_from_this()
    item.depth = depth;
    item.hasChildren = !children.empty();
    item.indexInFlatList = flatItems.size();
    flatItems.push_back(item);

    // Если узел раскрыт - добавляем детей
    if (item.isExpanded) {
        for (Entity* child : children) {
            Flatten(child, depth + 1);
        }
    }
}

// Эффективное обновление после переключения
void TreeViewModel::RebuildFrom(size_t startIndex) {
    // Оставляем элементы до startIndex, удаляем все после
    // и заново добавляем с текущего узла
    auto& item = flatItems[startIndex];
    flatItems.erase(flatItems.begin() + startIndex + 1, flatItems.end());

    // Добавляем детей, если раскрыт
    if (item.isExpanded && item.object) {
        for (Entity* child : item.object->GetChildren()) {
            Flatten(child, item.depth + 1);
        }
    }
}