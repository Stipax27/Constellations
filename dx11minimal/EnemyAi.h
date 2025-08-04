
point3d flyDirectionEnemy = { 0, 0, 0 };

namespace Enemy
{
    enum class AIState {
        PATROL,
        CHASE,
        ATTACK,
        SEARCH
    };

    class EnemyAI {
    public:
        AIState currentState;
        float playerDistance;
        bool playerVisible;
        int currentWaypoint;
        float patrolSpeed;
        float chaseSpeed;
        float rotationSpeed;
        point3d currentDirection;

        XMVECTOR currentRotation = XMQuaternionIdentity();
        XMVECTOR ForwardEn = XMVectorSet(0, 0, 1, 0);
        XMVECTOR defaultUp = XMVectorSet(0, 1, 0, 0);  // Исправлено на (0,1,0) для корректной системы координат
        XMVECTOR RightEn = XMVectorSet(1, 0, 0, 0);    // Исправлено на (1,0,0)
        XMVECTOR UpEn;

        std::vector<point3d> waypoints;
        XMMATRIX enemyConstellationOffset = XMMatrixIdentity();  // Инициализировано как единичная матрица

        EnemyAI() :
            currentState(AIState::PATROL),
            playerDistance(0.0f),
            playerVisible(false),
            currentWaypoint(0),
            patrolSpeed(5.f),
            chaseSpeed(10.0f),
            rotationSpeed(0.1f)
        {
            waypoints = {
                point3d(-50000.0f, 1000.0f, 0.0f),
                point3d(0.0f, 1000.0f, 50000.0f),
                point3d(50000.0f, 1000.0f, 25000.0f)
            };

           
            UpEn = XMVector3Rotate(defaultUp, currentRotation);
        }

        void AiUpdate(float deltaTime) {
            point3d currentPos = enemyPositions;
            point3d heroPos = heroPosition );

            // Обновляем расстояние до игрока
            playerDistance = (heroPos - currentPos).magnitude();

            // Здесь можно добавить логику определения видимости игрока
            // playerVisible = CheckPlayerVisibility(heroPos);

            switch (currentState) {
            case AIState::PATROL:
                Patrol(deltaTime);
                if (playerVisible) currentState = AIState::CHASE;
                break;

            case AIState::CHASE:
                Chase(heroPos);
                if (playerDistance < 500.0f) currentState = AIState::ATTACK;
                else if (!playerVisible) {
                    currentState = AIState::SEARCH;
                    waypoints.push_back(currentPos); // Запоминаем последнюю известную позицию
                }
                break;

                // Другие состояния можно раскомментировать по мере реализации
            }
        }

        // Метод для получения текущей позиции врага
        point3d GetPosition() const {
            return point3d(
                enemyConstellationOffset.r[3].m128_f32[0],
                enemyConstellationOffset.r[3].m128_f32[1],
                enemyConstellationOffset.r[3].m128_f32[2]
            );
        }

    private:
        void Patrol(float deltaTime) {
            if (waypoints.empty()) return;

            point3d currentPos = GetPosition();
            point3d& target = waypoints[currentWaypoint];
            point3d direction = target - currentPos;
            float distance = direction.magnitude();

            if (distance < 20.0f) {
                currentWaypoint = (currentWaypoint + 1) % waypoints.size();
                return;
            }

            point3d moveDir = direction.normalized() * patrolSpeed * deltaTime;
            enemyConstellationOffset = XMMatrixMultiply(
                enemyConstellationOffset,
                XMMatrixTranslation(moveDir.x, moveDir.y, moveDir.z)
            );

            UpdateRotation(direction);
        }

        void Chase(const point3d& heroPos) {
            point3d currentPos = GetPosition();
            point3d direction = (heroPos - currentPos).normalized();

            // Обновляем currentDirection для использования в других методах
            currentDirection = direction;

            // Двигаем врага в направлении героя
            enemyConstellationOffset = XMMatrixMultiply(
                enemyConstellationOffset,
                XMMatrixTranslation(direction.x * chaseSpeed,
                    direction.y * chaseSpeed,
                    direction.z * chaseSpeed)
            );

            UpdateRotation(direction);
            drawString("chasing player", window.width / 3, window.height / 3, 1.f, true);
        }

        void UpdateRotation(const point3d& direction) {
            if (direction.magnitude() > 0.1f) {
                XMVECTOR targetDir = XMVectorSet(direction.x, direction.y, direction.z, 0.0f);
                targetDir = XMVector3Normalize(targetDir);

                float dot = XMVectorGetX(XMVector3Dot(ForwardEn, targetDir));
                if (fabsf(dot) < 0.9999f) {
                    XMVECTOR axis = XMVector3Cross(ForwardEn, targetDir);
                    axis = XMVector3Normalize(axis);
                    float angle = acosf(dot);
                    XMVECTOR rotQuat = XMQuaternionRotationAxis(axis, angle);

                    currentRotation = XMQuaternionMultiply(currentRotation, rotQuat);
                    currentRotation = XMQuaternionNormalize(currentRotation);

                    // Обновляем базовые векторы
                    ForwardEn = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), currentRotation);
                    UpEn = XMVector3Rotate(defaultUp, currentRotation);
                    RightEn = XMVector3Cross(UpEn, ForwardEn);
                }
            }
        }
    };

    static EnemyAI enemyAi;
}


void updateEnemyPosition(float deltaTime) {
    Enemy::enemyAi.AiUpdate(deltaTime);
}