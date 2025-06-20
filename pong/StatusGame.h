extern std::vector<Constellation*> starSet;
extern ZodiacSign player_sign;
extern ZodiacSign currentEnemyID;
extern DWORD currentTime;

float getConstellationHP(const Entity& entity) { // Возвращаем ХП
    return entity.getHP();
}

struct BattleState {
    DWORD timestamp;
    int playerHP;
    int enemyHP;
    std::vector<int> playerStarsHealth;
    std::vector<int> enemyStarsHealth;
    std::vector<point3d> playerstarCords;
    std::vector<point3d> enemystarCords;
    point3d player_dodge_ofs;
    point3d starfield_angles;
    bool isAnchorPoint = false;

    bool operator==(const BattleState& other) const {
        return timestamp == other.timestamp &&
            playerHP == other.playerHP &&
            enemyHP == other.enemyHP &&
            player_dodge_ofs == other.player_dodge_ofs &&
            starfield_angles == other.starfield_angles &&
            playerStarsHealth == other.playerStarsHealth &&
            enemyStarsHealth == other.enemyStarsHealth &&
            playerstarCords == other.playerstarCords &&
            enemystarCords == other.enemystarCords;
    }
};

std::vector<BattleState> battleHistory; // Вектор истории
const size_t MAX_HISTORY_STATES = 18000; // Максимальное ограничение на запись истории
size_t currentStateIndex = 0;
size_t anchorIndex = 0;
bool hasAnchorPoint = false;

DWORD lastRewindTime = 0;
const DWORD REWIND_INTERVAL = 1; // 1ms между шагами перемотки
const int REWIND_SPEED_MULTIPLIER = 2; // Во сколько раз быстрее будет отмотка

void SaveCurrentState(bool forceSave = false, bool asAnchorPoint = false) {
    Entity* playerEntity = &entities[static_cast<int>(player_sign)];
    Entity* enemyEntity = &entities[static_cast<int>(currentEnemyID)];

    BattleState currentState;
    currentState.timestamp = currentTime;
    currentState.playerHP = playerEntity->getHP();
    currentState.enemyHP = enemyEntity->getHP();

    
    if (!forceSave && !battleHistory.empty()) {// Сохранение состояния только если оно изменилось 
        const BattleState& lastState = battleHistory.back();
        if (lastState == currentState) {
            return; // Пропускаем одинаковые состояния
        }
    }

    
    currentState.playerStarsHealth = playerEntity->healthSystem->starsHealth;
    currentState.playerstarCords = playerEntity->constellation->starsCords;
    currentState.enemyStarsHealth = enemyEntity->healthSystem->starsHealth;
    currentState.enemystarCords = enemyEntity->constellation->starsCords;
    currentState.player_dodge_ofs = player_dodge_ofs;
    currentState.starfield_angles = starfield_angles;

    if (asAnchorPoint) {
        currentState.isAnchorPoint = true;
        hasAnchorPoint = true;
        anchorIndex = battleHistory.size();
    }

    battleHistory.push_back(currentState);
    currentStateIndex = battleHistory.size() - 1;

    
    if (battleHistory.size() > MAX_HISTORY_STATES) {// Ограничиваем размер истории
        if (hasAnchorPoint && anchorIndex == 0) {
            battleHistory.erase(battleHistory.begin() + 1);
            anchorIndex--;
        }
        else {
            battleHistory.erase(battleHistory.begin());
            if (hasAnchorPoint) anchorIndex--;
        }
        currentStateIndex--;
    }
}

bool RewindToState(size_t targetIndex) {
    if (targetIndex >= battleHistory.size()) {
        return false;
    }

    Entity* playerEntity = &entities[static_cast<int>(player_sign)];
    Entity* enemyEntity = &entities[static_cast<int>(currentEnemyID)];

    const BattleState& targetState = battleHistory[targetIndex];

    // Игрок откат
    playerEntity->healthSystem->starsHealth = targetState.playerStarsHealth;
    playerEntity->healthSystem->updateStarsHP();
    playerEntity->constellation->starsCords = targetState.playerstarCords;

    // Враг откат
    enemyEntity->healthSystem->starsHealth = targetState.enemyStarsHealth;
    enemyEntity->healthSystem->updateStarsHP();
    enemyEntity->constellation->starsCords = targetState.enemystarCords;

    // Уклонение и Угол откат 
    player_dodge_ofs = targetState.player_dodge_ofs;
    starfield_angles = targetState.starfield_angles;

    currentStateIndex = targetIndex;
    return true;
}

bool RewindOneStepBack() {// Установка точки за которую нельзя отмататься 
    if (currentStateIndex == 0 || (hasAnchorPoint && currentStateIndex <= anchorIndex)) {
        return false;
    }

    
    size_t newIndex = currentStateIndex;// Пропускаем несколько состояний для ускорения отмотки
    if (currentStateIndex > REWIND_SPEED_MULTIPLIER) {
        newIndex -= REWIND_SPEED_MULTIPLIER;
        
        if (hasAnchorPoint && newIndex <= anchorIndex) {// Проверка на пропуск точки 
            newIndex = anchorIndex + 1;
        }
    }
    else {
        newIndex = 0;
    }

    return RewindToState(newIndex);
}

void ResetTimeAnchor() {
    if (!battleHistory.empty()) {
        BattleState newAnchor = battleHistory[currentStateIndex];
        newAnchor.isAnchorPoint = true;

       
        battleHistory.resize(currentStateIndex + 1); // Очистка истории после текущего состояния

        
        battleHistory[currentStateIndex] = newAnchor; // Постановка точки заново
        anchorIndex = currentStateIndex;
        hasAnchorPoint = true;
    }
}

void UpdateRewind() { // Установка скорости для отката
    DWORD currentTick = GetTickCount();
    if (currentTick - lastRewindTime >= REWIND_INTERVAL) {
        if (RewindOneStepBack()) {
            lastRewindTime = currentTick;
        }
    }
}