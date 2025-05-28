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

    currentState.playerHP = playerEntity->getHP();
    currentState.playerStarsHealth = playerEntity->healthSystem->starsHealth;
    currentState.playerstarCords = playerEntity->constellation->starsCords;

    currentState.enemyHP = enemyEntity->getHP();
    currentState.enemyStarsHealth = enemyEntity->healthSystem->starsHealth;
    currentState.enemystarCords = enemyEntity->constellation->starsCords;

    currentState.player_dodge_ofs = player_dodge_ofs;
    currentState.starfield_angles = starfield_angles;

    if (battleHistory.empty() || battleHistory.back() != currentState) {
        battleHistory.push_back(currentState);
        currentStateIndex = battleHistory.size() - 1;

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

    Entity* playerEntity = &entities[static_cast<int>(player_sign)];
    Entity* enemyEntity = &entities[static_cast<int>(currentEnemyID)];

    int stepsToRewind = min(4, static_cast<int>(currentStateIndex));
    currentStateIndex -= stepsToRewind;
    const BattleState& prevState = battleHistory[currentStateIndex];

    playerEntity->healthSystem->starsHealth = prevState.playerStarsHealth;
    playerEntity->healthSystem->updateStarsHP();
    playerEntity->constellation->starsCords = prevState.playerstarCords;

    enemyEntity->healthSystem->starsHealth = prevState.enemyStarsHealth;
    enemyEntity->healthSystem->updateStarsHP();
    enemyEntity->constellation->starsCords = prevState.enemystarCords;

    player_dodge_ofs = prevState.player_dodge_ofs;
    starfield_angles = prevState.starfield_angles;

    return true;
}