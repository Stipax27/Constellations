
extern std::vector<Constellation*> starSet;
extern ZodiacSign player_sign;
extern ZodiacSign currentEnemyID;
extern DWORD currentTime;

struct BattleState{
    DWORD timestamp;
    float playerHP;
    float enemyHP;
    std::vector<float> playerStarsHealth;
    std::vector<float> enemyStarsHealth;
    point3d playerPos;
    point3d enemyPos;
};

std::vector<BattleState> battleHistory;
BattleState lastSavedState;

void SaveCurrentState() {
    BattleState currentState;
    currentState.timestamp = timeGetTime();

    currentState.playerHP = getConstellationHP(*starSet[player_sign]);
    currentState.playerStarsHealth = starSet[player_sign]->starsHealth;
    /*currentState.playerPos = starSet[player_sign]->starsRenderedCords[0];*/

    currentState.enemyHP = getConstellationHP(*starSet[currentEnemyID]);
    currentState.enemyStarsHealth = starSet[currentEnemyID]->starsHealth;
    /*currentState.enemyPos = starSet[currentEnemyID]->starsRenderedCords[0];*/


    if (battleHistory.empty() ||
        currentState.playerHP != lastSavedState.playerHP ||
        currentState.enemyHP != lastSavedState.enemyHP ||

        currentState.playerStarsHealth != lastSavedState.playerStarsHealth ||
        currentState.enemyStarsHealth != lastSavedState.enemyStarsHealth)

    {

        battleHistory.push_back(currentState);
        lastSavedState = currentState;

        if (battleHistory.size() > 100) {
            battleHistory.erase(battleHistory.begin());
        }
    }
    
}


bool RewindTime(DWORD targetTime) {
    if (battleHistory.empty()) {
        
        return false;
    }

    for (auto it = battleHistory.rbegin(); it != battleHistory.rend(); ++it) {
        if (it->timestamp <= targetTime) {
            
            starSet[player_sign]->starsHealth = it->playerStarsHealth;
            starSet[currentEnemyID]->starsHealth = it->enemyStarsHealth;

           
            /*getConstellationHP(*starSet[player_sign], it->playerHP);
            getConstellationHP(*starSet[currentEnemyID], it->enemyHP);

            
            starSet[player_sign]->starsRenderedCords[0] = it->playerPos;
            starSet[currentEnemyID]->starsRenderedCords[0] = it->enemyPos;*/

            
          
        }
    }

    
    return true;
}
