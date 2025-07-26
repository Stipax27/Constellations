
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
        float patrolSpeed = 0.5f;
        float chaseSpeed;
        float rotationSpeed;
        point3d currentDirection;

        XMVECTOR currentRotation = XMQuaternionIdentity();
        XMVECTOR ForwardEn = XMVectorSet(0, 0, 1, 0);
        XMVECTOR defaultUp = XMVectorSet(0, -1, 0, 0);
        XMVECTOR RightEn = XMVectorSet(-1, 0, 0, 0);
        XMVECTOR UpEn = XMVector3Rotate(defaultUp, currentRotation);

        std::vector<point3d> waypoints;
        XMMATRIX enemyConstellationOffset = XMMatrixTranslation(0, 0, 0);

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
       
       }
        
                void AiUpdate(float deltaTime) {
        
                    
                    switch (currentState) {
                    case AIState::PATROL:
                        Patrol(deltaTime);
                        //if (playerVisible) currentState = AIState::CHASE;
                        break;
        
                        /*case AIState::CHASE:
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
                            break;*/
                    }
                }
        
            private:
        
                void Patrol(float deltaTime) {
                    if (waypoints.empty()) return;

                    // Получаем текущую позицию врага
                    point3d currentPos = point3d(
                        enemyConstellationOffset.r[3].m128_f32[0],
                        enemyConstellationOffset.r[3].m128_f32[1],
                        enemyConstellationOffset.r[3].m128_f32[2]
                    );

                    point3d& target = waypoints[currentWaypoint];
                    point3d direction = target - currentPos;
                    float distance = direction.magnitude();

                    // Если близко к точке - переключаемся на следующую
                    if (distance < 20.0f) {
                        currentWaypoint = (currentWaypoint + 1) % waypoints.size();
                        return;
                    }

                    // Нормализуем направление и применяем скорость с учетом deltaTime
                    point3d moveDir = direction.normalized() * patrolSpeed * deltaTime;

                    // Обновляем позицию врага
                    enemyConstellationOffset = XMMatrixMultiply(
                        enemyConstellationOffset,
                        XMMatrixTranslation(moveDir.x, moveDir.y, moveDir.z)
                    );

                    // Поворот только если есть куда поворачивать
                    if (direction.magnitude() > 0.1f) {
                        XMVECTOR targetDir = XMVectorSet(direction.x, direction.y, direction.z, 0.0f);
                        targetDir = XMVector3Normalize(targetDir);

                        // Безопасный расчет поворота
                        float dot = XMVectorGetX(XMVector3Dot(ForwardEn, targetDir));
                        if (fabsf(dot) < 0.9999f) { // Если не параллельны
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
        //
        //        void Chase(const point3d& playerPos, point3d& enemyPos) {
        //            point3d direction = (playerPos - enemyPos).normalized();
        //
        //            enemyPos += currentDirection * chaseSpeed;
        //
        //
        //            drawString("Chasing player", window.width / 3, window.height / 3, 1.f, true);
        //        }
        //
        //        void Attack(const point3d& playerPos, point3d& enemyPos) {
        //
        //            point3d direction = (playerPos - enemyPos).normalized();
        //
        //            drawString("Attacking player", window.width / 3, window.height / 3, 1.f, true);
        //        }
        //
        //        void Search(const point3d& lastKnownPos, point3d& enemyPos) {
        //            if (waypoints.size() <= 3) return;
        //
        //            point3d target = waypoints.back();
        //            point3d direction = (target - enemyPos).normalized();
        //
        //            float distance = (target - enemyPos).magnitude();
        //            if (distance < 30.0f) {
        //                waypoints.pop_back();
        //            }
        //
        //            drawString("Searching player", window.width / 3, window.height / 3, 1.f, true);
        //        }
        //
    }static enemyAi;
}


void updateEnemyPosition(float deltaTime) {
    Enemy::enemyAi.AiUpdate(deltaTime);
}