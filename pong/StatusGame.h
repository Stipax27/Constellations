//extern std::vector<Constellation*> starSet;
//extern ZodiacSign player_sign;
//extern ZodiacSign currentEnemyID;
//extern DWORD currentTime;
//
//float getConstellationHP(const Constellation& constellation) {
//    return constellation.hp;
//}
//
//struct BattleState {
//    DWORD timestamp;
//    float playerHP;
//    float enemyHP;
//    std::vector<float> playerStarsHealth;
//    std::vector<float> enemyStarsHealth;
//
//    std::vector<point3d>playerstarCords;
//    std::vector<point3d>enemystarCords;
//
//    point3d playerAngel;
//    point3d enemyAngel;
//
//    /*bool operator!=(const BattleState& other) const {
//        return timestamp != other.timestamp ||
//            playerHP != other.playerHP ||
//            enemyHP != other.enemyHP ||
//            playerStarsHealth != other.playerStarsHealth ||
//            enemyStarsHealth != other.enemyStarsHealth ||
//            playerstarCords != other.playerstarCords ||
//            enemystarCords != other.enemystarCords;
//    }*/
//};
//
//
//std::vector<BattleState> battleHistory;
//BattleState lastSavedState;
//size_t currentStateIndex = 0;  
//
//void SaveCurrentState() {
//
//    if (!starSet[player_sign] || !starSet[currentEnemyID]) return;
//
//    BattleState currentState;
//    currentState.timestamp = timeGetTime();
//
//    currentState.playerHP = getConstellationHP(*starSet[player_sign]);
//    currentState.playerStarsHealth = starSet[player_sign]->starsHealth;
//    currentState.playerstarCords = starSet[player_sign]->starsCords;
//
//    currentState.enemyHP = getConstellationHP(*starSet[currentEnemyID]);
//    currentState.enemyStarsHealth = starSet[currentEnemyID]->starsHealth;
//    currentState.enemystarCords = starSet[currentEnemyID]->starsCords;
//
//    if (battleHistory.empty()) {
//        battleHistory.push_back(currentState);
//        lastSavedState = currentState;
//        currentStateIndex = 0;
//        return;
//    }
//
//    if (currentState != lastSavedState) {
//        if (currentStateIndex < battleHistory.size() - 1) {
//            battleHistory.erase(battleHistory.begin() + currentStateIndex + 1, battleHistory.end());
//        }
//
//        battleHistory.push_back(currentState);
//        lastSavedState = currentState;
//        currentStateIndex = battleHistory.size() - 1;
//
//        if (battleHistory.size() > 18000) {
//            battleHistory.erase(battleHistory.begin());
//            currentStateIndex--;
//        }
//    }
//}
//bool RewindOneStepBack() {
//    if (battleHistory.empty() || currentStateIndex == 0) {
//        return false;
//    }
//
//    currentStateIndex--;
//    const BattleState& prevState = battleHistory[currentStateIndex];
//
//    starSet[player_sign]->starsHealth = prevState.playerStarsHealth;
//    starSet[player_sign]->starsCords = prevState.playerstarCords;  
//    starSet[player_sign]->hp = prevState.playerHP;
//    starSet[player_sign]->setStarsRenderedCords(
//        starSet[player_sign]->angle.x,
//        starSet[player_sign]->angle.y,
//        starSet[player_sign]->angle.z
//    );
//
//    starSet[currentEnemyID]->starsHealth = prevState.enemyStarsHealth;
//    starSet[currentEnemyID]->starsCords = prevState.enemystarCords;  
//    starSet[currentEnemyID]->hp = prevState.enemyHP;
//    starSet[currentEnemyID]->setStarsRenderedCords(
//        starSet[currentEnemyID]->angle.x,
//        starSet[currentEnemyID]->angle.y,
//        starSet[currentEnemyID]->angle.z
//    );
//
//    lastSavedState = prevState;
//    return true;
//}