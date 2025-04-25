extern std::vector<Constellation*> starSet;
extern ZodiacSign player_sign;
extern ZodiacSign currentEnemyID;
extern DWORD currentTime;

float getConstellationHP(const Constellation& constellation) {
    return constellation.hp;
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
    if (!starSet[player_sign] || !starSet[currentEnemyID]) return;

    BattleState currentState;
    currentState.timestamp = timeGetTime();

    
    currentState.playerHP = starSet[player_sign]->hp;
    currentState.playerStarsHealth = starSet[player_sign]->starsHealth;
    currentState.playerstarCords = starSet[player_sign]->starsCords;
     
    currentState.enemyHP = starSet[currentEnemyID]->hp;
    currentState.enemyStarsHealth = starSet[currentEnemyID]->starsHealth;
    currentState.enemystarCords = starSet[currentEnemyID]->starsCords;
    currentState.player_dodge_ofs = player_dodge_ofs;
    currentState.starfield_angles = starfield_angles;
   
    if (battleHistory.empty()) {
        battleHistory.push_back(currentState);
        currentStateIndex = 0;
        return;
    }

    
    //if (currentState != battleHistory[currentStateIndex]) 
    {
        
        //if (currentStateIndex < battleHistory.size() - 1) {
         //   battleHistory.erase(battleHistory.begin() + currentStateIndex + 1, battleHistory.end());
       // }

        
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

    BattleState prevState;
    for (int i = 0; i < 4; i++)
    {
        
        if (currentStateIndex > 0)
        {
            currentStateIndex -= 1;
            prevState = battleHistory[currentStateIndex];
            battleHistory.pop_back();
        }
    }

    starSet[player_sign]->hp = prevState.playerHP;
    starSet[player_sign]->starsHealth = prevState.playerStarsHealth;
    starSet[player_sign]->starsCords = prevState.playerstarCords;
     
    starSet[currentEnemyID]->hp = prevState.enemyHP;
    starSet[currentEnemyID]->starsHealth = prevState.enemyStarsHealth;
    starSet[currentEnemyID]->starsCords = prevState.enemystarCords;
    
    player_dodge_ofs = prevState.player_dodge_ofs;
    starfield_angles = prevState.starfield_angles;

    return true;
}