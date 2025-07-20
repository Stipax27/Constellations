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

    std::vector<point3d> waypoints;

public:
    EnemyAI() : currentState(AIState::PATROL), playerDistance(0.0f), playerVisible(false), currentWaypoint(0) {
        waypoints = {
            point3d(200.0f, 100.0f, 200.0f),
            point3d(100.0f, 10.0f, 50.0f),
            point3d(50.0f, 200.0f, 100.0f)
        };
    }

    void Update(const point3d& playerPos, point3d& enemyPos) {
        // Расстояние до игрока
        playerDistance = enemyPos.DistanceTo(playerPos);
        playerVisible = (playerDistance < 15.0f);

        // Логика состояний
        switch (currentState) {
        case AIState::PATROL:
            Patrol(enemyPos);
            if (playerVisible) currentState = AIState::CHASE;
            break;

        case AIState::CHASE:
            Chase(playerPos, enemyPos);
            if (playerDistance < 5.0f) currentState = AIState::ATTACK;
            else if (!playerVisible) currentState = AIState::SEARCH;
            break;

        case AIState::ATTACK:
            Attack();
            if (playerDistance > 5.0f) currentState = AIState::CHASE;
            break;

        case AIState::SEARCH:
            Search();
            if (playerVisible) currentState = AIState::CHASE;
            else if (playerDistance > 20.0f) currentState = AIState::PATROL;
            break;
        }
    }

    void Patrol(point3d& enemyPos) {
        const point3d& target = waypoints[currentWaypoint];
        point3d direction = target - enemyPos;

        float distance = direction.magnitude();

        if (distance < 2.0f) {
            currentWaypoint = (currentWaypoint + 1) % waypoints.size();
        }
        else {
            // Используем normalized() из вашего класса
            enemyPos += direction.normalized() * 0.02f;
        }

        drawString("Patrolling to waypoint ", window.width / 3, window.height / 3, 1.f, true);

    }

    void Chase(const point3d& playerPos, point3d& enemyPos) {
        point3d direction(playerPos.x - enemyPos.x,
            playerPos.y - enemyPos.y,
            playerPos.z - enemyPos.z);

        float distance = direction.DistanceTo(point3d());
        if (distance > 0) {
            float speed = 0.03f;
            enemyPos.x += direction.x / distance * speed;
            enemyPos.y += direction.y / distance * speed;
            enemyPos.z += direction.z / distance * speed;
        }
    }

    void Attack() {
        std::cout << "Attacking player!\n";
    }

    void Search() {
        std::cout << "Player lost. Searching...\n";
    }
};

