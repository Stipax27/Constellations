
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
                void AiUpdate() {
        
                    
                    switch (currentState) {
                    case AIState::PATROL:
                        Patrol();
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
        //
        //    private:
        //
                void Patrol() {
                    if (waypoints.empty()) return;

                    
                    point3d& target = waypoints[currentWaypoint];
                    point3d direction = target - flyDirectionEnemy;
                    float distance = direction.magnitude();
        
                    if (distance < 20.0f) {
                        currentWaypoint = (currentWaypoint + 1) % waypoints.size();
                    }
                    else {
        
                        flyDirectionEnemy += (direction * patrolSpeed);
                    }
        
                    string pos = flyDirectionEnemy.c_str();
                    drawString("Patrolling to waypoint " + (currentWaypoint),
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
    XMVECTOR currentPos = Enemy::enemyAi.enemyConstellationOffset.r[3];

  Enemy::enemyAi.enemyConstellationOffset = XMMatrixMultiply(
            Enemy::enemyAi.enemyConstellationOffset,
            XMMatrixTranslation(
                flyDirectionEnemy.x * Enemy::enemyAi.patrolSpeed * deltaTime,
                flyDirectionEnemy.y * Enemy::enemyAi.patrolSpeed * deltaTime,
                flyDirectionEnemy.z * Enemy::enemyAi.patrolSpeed * deltaTime
            )
        );
  
}