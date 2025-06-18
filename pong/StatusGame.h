extern std::vector<Constellation*> starSet;
extern ZodiacSign player_sign;
extern ZodiacSign currentEnemyID;
extern DWORD currentTime;

float getConstellationHP(const Entity& entity) {
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

const size_t MAX_HISTORY_STATES = 18000;
std::vector<BattleState> battleHistory;
size_t currentStateIndex = 0;
size_t anchorIndex = 0;
bool hasAnchorPoint = false;

DWORD lastRewindTime = 0;
const DWORD REWIND_INTERVAL = 50; // 50ms между шагами перемотки

void SaveCurrentState(bool forceSave = false, bool asAnchorPoint = false) {
    Entity* playerEntity = &entities[static_cast<int>(player_sign)];
    Entity* enemyEntity = &entities[static_cast<int>(currentEnemyID)];

    BattleState currentState;
    currentState.timestamp = currentTime;
    currentState.playerHP = playerEntity->getHP();
    currentState.enemyHP = enemyEntity->getHP();

    // —охран€ем только если состо€ние изменилось или принудительно
    if (!forceSave && !battleHistory.empty()) {
        const BattleState& lastState = battleHistory.back();
        if (lastState == currentState) {
            return; // ѕропускаем идентичные состо€ни€
        }
    }

    // —охран€ем остальные параметры
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

    // ќграничиваем размер истории
    if (battleHistory.size() > MAX_HISTORY_STATES) {
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

    // ¬осстановление состо€ни€ игрока
    playerEntity->healthSystem->starsHealth = targetState.playerStarsHealth;
    playerEntity->healthSystem->updateStarsHP();
    playerEntity->constellation->starsCords = targetState.playerstarCords;

    // ¬осстановление состо€ни€ врага
    enemyEntity->healthSystem->starsHealth = targetState.enemyStarsHealth;
    enemyEntity->healthSystem->updateStarsHP();
    enemyEntity->constellation->starsCords = targetState.enemystarCords;

    // ¬осстановление других параметров
    player_dodge_ofs = targetState.player_dodge_ofs;
    starfield_angles = targetState.starfield_angles;

    currentStateIndex = targetIndex;
    return true;
}

bool RewindOneStepBack() {
    if (currentStateIndex == 0 || (hasAnchorPoint && currentStateIndex <= anchorIndex)) {
        return false;
    }

    return RewindToState(currentStateIndex - 1);
}

void ResetTimeAnchor() {
    if (!battleHistory.empty()) {
        BattleState newAnchor = battleHistory[currentStateIndex];
        newAnchor.isAnchorPoint = true;

        // ќчищаем историю после текущего состо€ни€
        battleHistory.resize(currentStateIndex + 1);

        // ќбновл€ем €корь
        battleHistory[currentStateIndex] = newAnchor;
        anchorIndex = currentStateIndex;
        hasAnchorPoint = true;
    }
}