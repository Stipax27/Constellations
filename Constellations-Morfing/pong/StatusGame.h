extern std::vector<Constellation*> starSet;

extern ZodiacSign player_sign;
extern ZodiacSign currentEnemyID;
extern DWORD currentTime;

float getConstellationHP(const Entity& entity) {
    return entity.getHP();
}

struct BattleState {
    DWORD timestamp;
    float playerHP;
    float enemyHP;
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

const size_t MAX_HISTORY_STATES = 18000;
std::vector <BattleState> battleHistory;
size_t historyStart = 0;
size_t historyEnd = 0;
size_t currentStateIndex = 0;
size_t anchorIndex = 0;
bool hasAnchorPoint = false;

bool isHistoryFull() {
    return (historyEnd + 1) % MAX_HISTORY_STATES == historyStart;
}

void SetAnchorPoint() {
    if (!battleHistory.empty()) {
        battleHistory[currentStateIndex].isAnchorPoint = true;
        hasAnchorPoint = true;
        anchorIndex = currentStateIndex;
    }
}

void SaveCurrentState(bool forceSave = false, bool asAnchorPoint = false) {
    // Получаем указатели на враг и игрока
    Entity* playerEntity = &entities[static_cast<int>(player_sign)];
    Entity* enemyEntity = &entities[static_cast<int>(currentEnemyID)];

    BattleState currentState;
    currentState.timestamp = currentTime;

    currentState.playerHP = playerEntity->getHP();
    currentState.playerStarsHealth = playerEntity->healthSystem->starsHealth;
    currentState.playerstarCords = playerEntity->constellation->starsCords;

    currentState.enemyHP = enemyEntity->getHP();
    currentState.enemyStarsHealth = enemyEntity->healthSystem->starsHealth;
    currentState.enemystarCords = enemyEntity->constellation->starsCords;

    currentState.player_dodge_ofs = player_dodge_ofs;
    currentState.starfield_angles = starfield_angles;

    if (asAnchorPoint) {
        currentState.isAnchorPoint = true;
        hasAnchorPoint = true;
        anchorIndex = battleHistory.size();  // Будет последним элементом после push_back
    }

    if (forceSave || battleHistory.empty() || battleHistory.back() != currentState) {
        battleHistory.push_back(currentState);
        currentStateIndex = battleHistory.size() - 1;

        if (battleHistory.size() > MAX_HISTORY_STATES) {
            if (hasAnchorPoint && anchorIndex == 0) {
                // Не удаляем якорную точку (первый элемент)
                battleHistory.erase(battleHistory.begin() + 1);
                anchorIndex--;  // Смещаем индекс, так как удалили элемент перед ним
            }
            else {
                battleHistory.erase(battleHistory.begin());
                if (hasAnchorPoint) anchorIndex--;
            }
            currentStateIndex--;
        }
    }
}

bool RewindToState(size_t targetIndex) {
    if (historyEnd == historyStart || targetIndex >= (historyEnd - historyStart) % MAX_HISTORY_STATES) {
        return false;
    }

    Entity* playerEntity = &entities[static_cast<int>(player_sign)];
    Entity* enemyEntity = &entities[static_cast<int>(currentEnemyID)];

    const BattleState& targetState = battleHistory[(historyStart + targetIndex) % MAX_HISTORY_STATES];

    // Восстановление состояния
    playerEntity->healthSystem->starsHealth = targetState.playerStarsHealth;
    playerEntity->healthSystem->updateStarsHP();
    playerEntity->constellation->starsCords = targetState.playerstarCords;

    enemyEntity->healthSystem->starsHealth = targetState.enemyStarsHealth;
    enemyEntity->healthSystem->updateStarsHP();
    enemyEntity->constellation->starsCords = targetState.enemystarCords;

    player_dodge_ofs = targetState.player_dodge_ofs;
    starfield_angles = targetState.starfield_angles;

    currentStateIndex = targetIndex;
    return true;
}

bool RewindOneStepBack() {
    if (currentStateIndex == 0 || (hasAnchorPoint && currentStateIndex == anchorIndex)) {
        return false;
    }

    size_t stepsBack = min(4, currentStateIndex - (hasAnchorPoint ? anchorIndex : 0));
    return RewindToState(currentStateIndex - stepsBack);
}

void ResetTimeAnchor() {
    if (!battleHistory.empty()) {
        // Делаем текущее состояние новой якорной точкой
        BattleState anchorState = battleHistory[currentStateIndex];
        anchorState.isAnchorPoint = true;

        // Очищаем историю и добавляем только якорное состояние
        battleHistory.clear();
        battleHistory.push_back(anchorState);

        currentStateIndex = 0;
        anchorIndex = 0;
        hasAnchorPoint = true;
    }
}