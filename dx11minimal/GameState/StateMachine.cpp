//#include "GameState.h"
//
//class StateMachine {
//private:
//    // Стек состояний для поддержки push/pop (паузы, модальные окна)
//    std::vector<std::unique_ptr<GameState>> states;
//    bool shouldQuit = false;
//
//public:
//    // Инициализация начальным состоянием
//    void init(std::unique_ptr<GameState> initialState) {
//        pushState(std::move(initialState));
//    }
//
//    // Смена состояния (замена текущего)
//    void changeState(std::unique_ptr<GameState> newState) {
//        if (!states.empty()) {
//            states.back()->exit();
//            states.pop_back();
//        }
//        pushState(std::move(newState));
//    }
//
//    // Добавление состояния поверх текущего (пауза)
//    void pushState(std::unique_ptr<GameState> newState) {
//        newState->setStateMachine(this);
//        states.push_back(std::move(newState));
//        states.back()->enter();
//    }
//
//    // Удаление верхнего состояния (возврат из паузы)
//    void popState() {
//        if (!states.empty()) {
//            states.back()->exit();
//            states.pop_back();
//        }
//
//        // Вызываем enter для состояния, которое стало активным
//        if (!states.empty()) {
//            states.back()->enter();
//        }
//    }
//
//    // Главный игровой цикл
//    void update(float dt) {
//        if (!states.empty()) {
//            states.back()->update(dt);
//        }
//    }
//
//    
//    void quit() {
//        shouldQuit = true;
//    }
//
//    bool isRunning() const {
//        return !shouldQuit && !states.empty();
//    }
//
//    // Очистка всех состояний при выходе
//    void cleanup() {
//        while (!states.empty()) {
//            states.back()->exit();
//            states.pop_back();
//        }
//    }
//
//    // Отладочная информация
//    void printStack() const {
//        std::cout << "State stack (top to bottom):\n";
//        for (auto it = states.rbegin(); it != states.rend(); ++it) {
//            std::cout << "  - " << (*it)->getName() << "\n";
//        }
//    }
//};