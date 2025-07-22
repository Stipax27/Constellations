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
        // Инициализация точек патрулирования
        waypoints = {
            point3d(2000.0f, 1000.0f, 200.0f),
            point3d(1000.0f, 100.0f, 500.0f),
            point3d(500.0f, 2000.0f, 1000.0f)
        };

        if (!waypoints.empty()) {
            currentDirection = (waypoints[0] - point3d(0, 0, 0)).normalized();
        }
    }

    XMMATRIX GetTransform() const { return enemyConstellationOffset; }

    void SetTransform(const XMMATRIX& transform) { enemyConstellationOffset = transform; }

    void AiUpdate(point3d playerPos, point3d& enemyPos, XMMATRIX& enemyTransform) {
        // Обновляем расстояние и видимость игрока
        playerDistance = (enemyPos - playerPos).magnitude();
        playerVisible = (playerDistance < 1500.0f); // Увеличил радиус обнаружения

        // Логика конечного автомата
        switch (currentState) {
        case AIState::PATROL:
            Patrol(enemyPos, enemyTransform);
            if (playerVisible) currentState = AIState::CHASE;
            break;

        case AIState::CHASE:
            Chase(playerPos, enemyPos, enemyTransform);
            if (playerDistance < 500.0f) currentState = AIState::ATTACK;
            else if (!playerVisible) {
                currentState = AIState::SEARCH;
                // Запоминаем последнюю позицию игрока
                waypoints.push_back(playerPos);
            }
            break;

        case AIState::ATTACK:
            Attack(playerPos, enemyPos, enemyTransform);
            if (playerDistance > 600.0f) currentState = AIState::CHASE;
            break;

        case AIState::SEARCH:
            Search(playerPos, enemyPos, enemyTransform);
            if (playerVisible) currentState = AIState::CHASE;
            else if (waypoints.size() <= 3) currentState = AIState::PATROL;
            break;
        }
    }

private:
    void Patrol(point3d& enemyPos, XMMATRIX& enemyTransform) {
        if (waypoints.empty()) return;

        point3d target = waypoints[currentWaypoint];
        point3d direction = (target - enemyPos).normalized();

        // Плавное вращение в направлении движения
        currentDirection = lerp(currentDirection, direction, rotationSpeed * 0.1f).normalized();

        // Движение к точке
        enemyPos += currentDirection * patrolSpeed;

        // Обновление матрицы трансформации
        UpdateEnemyTransform(enemyPos, currentDirection, enemyTransform);

        // Проверка достижения точки
        float distance = (target - enemyPos).magnitude();
        if (distance < 300.0f) {
            currentWaypoint = (currentWaypoint + 1) % waypoints.size();
        }

        // Отладочная информация
        drawString("Patrolling to waypoint " + (currentWaypoint),
            window.width / 3, window.height / 3, 1.f, true);
    }

    void Chase(const point3d& playerPos, point3d& enemyPos, XMMATRIX& enemyTransform) {
        point3d direction = (playerPos - enemyPos).normalized();

        // Плавное вращение к игроку
        currentDirection = lerp(currentDirection, direction, rotationSpeed).normalized();

        // Движение к игроку
        enemyPos += currentDirection * chaseSpeed;

        // Обновление матрицы трансформации
        UpdateEnemyTransform(enemyPos, currentDirection, enemyTransform);

        // Отладочная информация
        drawString("Chasing player", window.width / 3, window.height / 3, 1.f, true);
    }

    void Attack(const point3d& playerPos, point3d& enemyPos, XMMATRIX& enemyTransform) {
        // Здесь можно добавить логику атаки
        point3d direction = (playerPos - enemyPos).normalized();
        currentDirection = lerp(currentDirection, direction, rotationSpeed).normalized();

        // Обновление матрицы трансформации
        UpdateEnemyTransform(enemyPos, currentDirection, enemyTransform);

        // Отладочная информация
        drawString("Attacking player", window.width / 3, window.height / 3, 1.f, true);
    }

    void Search(const point3d& lastKnownPos, point3d& enemyPos, XMMATRIX& enemyTransform) {
        if (waypoints.size() <= 3) return;

        point3d target = waypoints.back();
        point3d direction = (target - enemyPos).normalized();

        currentDirection = lerp(currentDirection, direction, rotationSpeed * 0.5f).normalized();
        enemyPos += currentDirection * patrolSpeed;

        UpdateEnemyTransform(enemyPos, currentDirection, enemyTransform);

        // Если достигли последней известной позиции
        float distance = (target - enemyPos).magnitude();
        if (distance < 300.0f) {
            waypoints.pop_back(); // Удаляем временную точку поиска
        }

        drawString("Searching player", window.width / 3, window.height / 3, 1.f, true);
    }

    void UpdateEnemyTransform(const point3d& position, const point3d& direction, XMMATRIX& transform) {
        // Создаем матрицу вращения на основе направления
        XMVECTOR forward = XMVectorSet(direction.x, direction.y, direction.z, 0.0f);
        XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

        // Если направление почти вертикальное - используем альтернативный вектор "вверх"
        if (fabs(direction.y) > 0.9f) {
            up = XMVectorSet(0.0f, 0.0f, direction.y > 0 ? -1.0f : 1.0f, 0.0f);
        }

        XMVECTOR right = XMVector3Cross(up, forward);
        up = XMVector3Cross(forward, right);

        forward = XMVector3Normalize(forward);
        right = XMVector3Normalize(right);
        up = XMVector3Normalize(up);

        // Собираем матрицу трансформации
        transform.r[0] = right;
        transform.r[1] = up;
        transform.r[2] = forward;
        transform.r[3] = XMVectorSet(position.x, position.y, position.z, 1.0f);
    }

    // Функция для линейной интерполяции векторов
    point3d lerp(const point3d& a, const point3d& b, float t) {
        return point3d(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t
        );
    }
}static enemyAi;