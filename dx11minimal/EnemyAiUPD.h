namespace Enemy
{
    void EnemyAI::AiUpdate(float deltaTime, point3d& heroPosition, point3d& enemyPositions) {
        data.playerDistance = (heroPosition - enemyPositions).magnitude();
        data.playerVisible = (data.playerDistance < 1000.0f);

        switch (data.currentState) {
        case AIState::PATROL:
            Patrol(deltaTime, enemyPositions);
            if (data.playerVisible && data.playerDistance > 500.0f) {
                data.currentState = AIState::CHASE;
            }
            break;

        case AIState::CHASE:
            Chase(heroPosition, enemyPositions);
            /*if (!data.playerVisible) {
                data.currentState = AIState::SEARCH;
                data.waypoints.push_back(GetPosition());
            }*/
            break;
        }
    }

    float radOrbit = 10000;
    point3d currDir;
    point3d OrbTarget = point3d();
    float Angle;

    void EnemyAI::Patrol(float deltaTime, point3d& enemyPositions) {

        deltaTime /= 1000.0f;
        if (data.waypoints.empty()) return;

        
        point3d currentPos = enemyPositions;

        point3d target = point3d(0.f,0,0);

        if ((enemyPositions - OrbTarget).magnitude() < 500.f)
        {
            Angle += data.patrolSpeed * deltaTime;
            OrbTarget = target + point3d(sin(Angle), 0.f, cos(Angle)) * radOrbit;
        }


        point3d direction = OrbTarget - currentPos;
        float distance = direction.magnitude();
        direction = direction.normalized();
        currDir = currDir.lerp(currDir, direction, 0.05f);

        point3d moveDir = currDir * data.patrolSpeed * deltaTime;



        /*point3d& target = data.waypoints[data.currentWaypoint];


        if ((enemyPositions - OrbTarget).magnitude() < 10.f) 
        {
            Angle += data.patrolSpeed * deltaTime;
            OrbTarget = target + point3d(sin(Angle),0.f,cos(Angle))* radOrbit;
        }

        point3d direction = OrbTarget - currentPos;
        float distance = direction.magnitude();
        direction = direction.normalized();
        currDir = currDir.lerp(currDir, direction, 0.01f);

        if (distance <= 500.0f) 
            data.currentWaypoint = (data.currentWaypoint + 1) % data.waypoints.size();
        
        point3d moveDir = direction * data.patrolSpeed * deltaTime;*/
        data.enemyConstellationOffset = XMMatrixMultiply(
            data.enemyConstellationOffset,
            XMMatrixTranslation(moveDir.x, moveDir.y, moveDir.z)
        );
        UpdateRotation(direction);
        
    }

    void EnemyAI::Chase(point3d& heroPos, point3d& enemyPositions) {
        point3d currentPos = enemyPositions;
        point3d direction = (heroPos - currentPos).normalized();
        data.currentDirection = direction;

        data.enemyConstellationOffset = XMMatrixMultiply(
            data.enemyConstellationOffset,
            XMMatrixTranslation(
                direction.x * data.chaseSpeed * deltaTime,
                direction.y * data.chaseSpeed * deltaTime,
                direction.z * data.chaseSpeed * deltaTime)
        );

        UpdateRotation(direction);
        drawString("chasing player", window.width / 3, window.height / 3, 1.f, true);
    }

    void EnemyAI::UpdateRotation(point3d& direction) {
        if (direction.magnitude() > 0.1f) {
            XMVECTOR targetDir = XMVectorSet(direction.x, direction.y, direction.z, 0.0f);
            targetDir = XMVector3Normalize(targetDir);

            float dot = XMVectorGetX(XMVector3Dot(data.ForwardEn, targetDir));
            if (fabsf(dot) < 0.9999f) {
                XMVECTOR axis = XMVector3Cross(data.ForwardEn, targetDir);
                axis = XMVector3Normalize(axis);
                float angle = acosf(dot);
                XMVECTOR rotQuat = XMQuaternionRotationAxis(axis, angle);

                data.currentRotation = XMQuaternionMultiply(data.currentRotation, rotQuat);
                data.currentRotation = XMQuaternionNormalize(data.currentRotation);

                data.ForwardEn = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), data.currentRotation);
                data.UpEn = XMVector3Rotate(data.defaultUp, data.currentRotation);
                data.RightEn = XMVector3Cross(data.UpEn, data.ForwardEn);
            }
        }
    }
}

Enemy::EnemyAI enemyAI(enemyData);

void updateEnemyPosition(float deltaTime, point3d& heroPosition, point3d& enemyPositions) {
    enemyAI.AiUpdate(deltaTime, heroPosition, enemyPositions);
    
}