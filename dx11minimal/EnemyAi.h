
point3d flyDirectionEnemy = { 0, 0, 0 };
float patrolSpeed = 5.f;
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
        float patrolSpeed = 0.000005f;
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
           patrolSpeed(5.0f),
           chaseSpeed(10.0f),
           rotationSpeed(0.1f)
       {
       
           waypoints = {
               point3d(200.0f, 100.0f, 20.0f),
               point3d(100.0f, 10.0f, 50.0f),
               point3d(50.0f, 200.0f, 100.0f)
           };
       
       }
        //
        //
        //
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

                    if (distance < 20.0f) {
                        // Достигли точки - переходим к следующей
                        currentWaypoint = (currentWaypoint + 1) % waypoints.size();
                    }
                    else {
                        // Нормализуем направление и применяем скорость
                        point3d moveDir = direction.normalized() * patrolSpeed * deltaTime;

                        // Обновляем позицию врага
                        enemyConstellationOffset = XMMatrixMultiply(
                            enemyConstellationOffset,
                            XMMatrixTranslation(moveDir.x, moveDir.y, moveDir.z)
                        );

                        // Обновляем вектор направления для вращения
                        if (direction.magnitude() > 0.1f) {
                            XMVECTOR targetDir = XMVectorSet(direction.x, direction.y, direction.z, 0.0f);
                            targetDir = XMVector3Normalize(targetDir);

                            XMVECTOR axis = XMVector3Cross(ForwardEn, targetDir);
                            float angle = acosf(XMVectorGetX(XMVector3Dot(ForwardEn, targetDir)));
                            XMVECTOR rotQuat = XMQuaternionRotationAxis(axis, angle);

                            currentRotation = XMQuaternionMultiply(currentRotation, rotQuat);

                            // Обновляем базовые векторы
                            ForwardEn = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), currentRotation);
                            UpEn = XMVector3Rotate(defaultUp, currentRotation);
                            RightEn = XMVector3Cross(UpEn, ForwardEn);
                        }
                    }

                    drawString("Patrolling to waypoint " ,
                        window.width / 3, window.height / 3, 1.f, true);
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