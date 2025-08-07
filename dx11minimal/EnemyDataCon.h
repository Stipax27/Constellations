
namespace Enemy
{
    EnemyData::EnemyData() :
        currentState(AIState::PATROL),
        playerDistance(0.0f),
        playerVisible(false),
        currentWaypoint(0),
        patrolSpeed(5.f),
        chaseSpeed(100.0f),
        rotationSpeed(0.1f),
        patrolProgress(0.f),
        currentRotation(XMQuaternionIdentity()),
        ForwardEn(XMVectorSet(0, 0, 1, 0)),
        defaultUp(XMVectorSet(0, 1, 0, 0)),
        RightEn(XMVectorSet(1, 0, 0, 0)),
        enemyConstellationOffset(XMMatrixIdentity())
    {
        waypoints = {
            point3d(-50000.0f, 0.0f, 0.0f),
            point3d(0.0f, 0.0f, 50000.0f),
            point3d(50000.0f, 1000.0f, 25000.0f)
        };
        UpEn = XMVector3Rotate(defaultUp, currentRotation);
    }
}


Enemy::EnemyData enemyData;

