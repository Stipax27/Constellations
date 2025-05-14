extern std::vector<Constellation*> starSet;
extern std::vector<Entity> entities;  // Изменил на vector<Entity>
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
    std::vector<float> playerStarsHealth;
    std::vector<float> enemyStarsHealth;
    std::vector<point3d> playerstarCords;
    std::vector<point3d> enemystarCords;
    point3d player_dodge_ofs;
    point3d starfield_angles;

    bool operator!=(const BattleState& other) const {
        return playerHP != other.playerHP ||
            enemyHP != other.enemyHP ||
            playerStarsHealth != other.playerStarsHealth ||
            enemyStarsHealth != other.enemyStarsHealth ||
            playerstarCords != other.playerstarCords ||
            enemystarCords != other.enemystarCords ||
            player_dodge_ofs != other.player_dodge_ofs ||
            starfield_angles != other.starfield_angles;
    }
};

std::vector<BattleState> battleHistory;
BattleState lastSavedState;
size_t currentStateIndex = 0;

void SaveCurrentState() {
    // Получаем указатели на враг и игрока
    Entity* playerEntity = &entities[static_cast<int>(player_sign)];
    Entity* enemyEntity = &entities[static_cast<int>(currentEnemyID)];

    BattleState currentState;
    currentState.timestamp = currentTime;

    // Сохраняем состояние игрока
    currentState.playerHP = playerEntity->getHP();
    currentState.playerStarsHealth = playerEntity->constellation->healthSystem->starsHealth;
    currentState.playerstarCords = playerEntity->constellation->starsCords;

    // Сохраняем состояние врага
    currentState.enemyHP = enemyEntity->getHP();
    currentState.enemyStarsHealth = enemyEntity->constellation->healthSystem->starsHealth;
    currentState.enemystarCords = enemyEntity->constellation->starsCords;

    currentState.player_dodge_ofs = player_dodge_ofs;
    currentState.starfield_angles = starfield_angles;

    // Если история пуста или состояние изменилось
    if (battleHistory.empty() || battleHistory.back() != currentState) {
        battleHistory.push_back(currentState);
        currentStateIndex = battleHistory.size() - 1;

        // Ограничиваем размер истории
        if (battleHistory.size() > 18000) {
            battleHistory.erase(battleHistory.begin());
            currentStateIndex--;
        }
    }
}

bool RewindOneStepBack() {
    if (battleHistory.empty() || currentStateIndex == 0) {
        return false;
    }

    // Получаем указатели на текущие сущности
    Entity* playerEntity = &entities[static_cast<int>(player_sign)];
    Entity* enemyEntity = &entities[static_cast<int>(currentEnemyID)];

    // Перемещаемся на несколько шагов назад
    int stepsToRewind = min(4, static_cast<int>(currentStateIndex));
    currentStateIndex -= stepsToRewind;
    const BattleState& prevState = battleHistory[currentStateIndex];

    // Восстанавливаем состояние игрока
    playerEntity->constellation->healthSystem->starsHealth = prevState.playerStarsHealth;
    playerEntity->constellation->healthSystem->updateStarsHP();
    playerEntity->constellation->starsCords = prevState.playerstarCords;

    // Восстанавливаем состояние врага
    enemyEntity->constellation->healthSystem->starsHealth = prevState.enemyStarsHealth;
    enemyEntity->constellation->healthSystem->updateStarsHP();
    enemyEntity->constellation->starsCords = prevState.enemystarCords;

    player_dodge_ofs = prevState.player_dodge_ofs;
    starfield_angles = prevState.starfield_angles;

    return true;
}