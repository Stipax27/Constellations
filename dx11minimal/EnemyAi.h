enum class AIState {
    PATROL,
    CHASE,
    ATTACK,
    SEARCH
};

class EnemyAI {
private:
    AIState currentState;
    float playerDistance;
    bool playerVisible;
    int currentWaypoint;
    float patrolSpeed;
    float chaseSpeed;
    float rotationSpeed;
    point3d currentDirection;
    
    std::vector<point3d> waypoints;

public:
    XMMATRIX enemyConstellationOffset = XMMatrixTranslation(0, 0, 0);
    EnemyAI() :
        currentState(AIState::PATROL),
        playerDistance(0.0f),
        playerVisible(false),
        currentWaypoint(0),
        patrolSpeed(5.0f),
        chaseSpeed(10.0f),
        rotationSpeed(0.1f)
    {
       
        waypoints = {
            point3d(2000.0f, 1000.0f, 200.0f),
            point3d(1000.0f, 100.0f, 500.0f),
            point3d(500.0f, 2000.0f, 1000.0f)
        };
 
    }

    

    void AiUpdate(point3d playerPos, point3d& enemyPos) {
        
        playerDistance = (enemyPos - playerPos).magnitude();
        playerVisible = (playerDistance < 1500.0f); 

        
        switch (currentState) {
        case AIState::PATROL:
            Patrol(enemyPos);
            if (playerVisible) currentState = AIState::CHASE;
            break;

        case AIState::CHASE:
            Chase(playerPos, enemyPos );
            if (playerDistance < 500.0f) currentState = AIState::ATTACK;
            else if (!playerVisible) {
                currentState = AIState::SEARCH;
                
                waypoints.push_back(playerPos);
            }
            break;

        case AIState::ATTACK:
            Attack(playerPos, enemyPos);
            if (playerDistance > 600.0f) currentState = AIState::CHASE;
            break;

        case AIState::SEARCH:
            Search(playerPos, enemyPos);
            if (playerVisible) currentState = AIState::CHASE;
            else if (waypoints.size() <= 3) currentState = AIState::PATROL;
            break;
        }
    }

private:

    void Patrol(point3d& enemyPos) {
        if (waypoints.empty()) return;

        point3d target = waypoints[currentWaypoint];
        point3d direction = (target - enemyPos).normalized();

        enemyPos += currentDirection * patrolSpeed;

        float distance = (target - enemyPos).magnitude();
        if (distance < 300.0f) {
            currentWaypoint = (currentWaypoint + 1) % waypoints.size();
        }

       
        drawString("Patrolling to waypoint " + (currentWaypoint),
            window.width / 3, window.height / 3, 1.f, true);
    }

    void Chase(const point3d& playerPos, point3d& enemyPos) {
        point3d direction = (playerPos - enemyPos).normalized();

        enemyPos += currentDirection * chaseSpeed;

       
        drawString("Chasing player", window.width / 3, window.height / 3, 1.f, true);
    }

    void Attack(const point3d& playerPos, point3d& enemyPos) {
       
        point3d direction = (playerPos - enemyPos).normalized();
   
        drawString("Attacking player", window.width / 3, window.height / 3, 1.f, true);
    }

    void Search(const point3d& lastKnownPos, point3d& enemyPos) {
        if (waypoints.size() <= 3) return;

        point3d target = waypoints.back();
        point3d direction = (target - enemyPos).normalized();

        float distance = (target - enemyPos).magnitude();
        if (distance < 30.0f) {
            waypoints.pop_back(); 
        }

        drawString("Searching player", window.width / 3, window.height / 3, 1.f, true);
    }

}static enemyAi;