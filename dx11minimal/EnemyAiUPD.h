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

        if (data.waypoints.empty()) return;

        
        point3d currentPos = enemyPositions;

        point3d& target = data.waypoints[data.currentWaypoint];
        point3d direction = target - currentPos;
        float distance = direction.magnitude();

        //currDir = direction.lerp(sin(Angle),0.f,cos(Angle))* radOrbit;
        if (distance < 200.0f) {
            data.currentWaypoint = (data.currentWaypoint + 1) % data.waypoints.size();
            return;
        }

        // Нормализуем направление и применяем скорость с учетом deltaTime
        point3d moveDir = direction.normalized() * data.patrolSpeed * deltaTime;

        // Обновляем позицию врага
        data.enemyConstellationOffset = XMMatrixMultiply(
            data.enemyConstellationOffset,
            XMMatrixTranslation(moveDir.x, moveDir.y, moveDir.z)
        );

        // Поворот только если есть куда поворачивать
        if (direction.magnitude() > 0.1f) {
            XMVECTOR targetDir = XMVectorSet(direction.x, direction.y, direction.z, 0.0f);
            targetDir = XMVector3Normalize(targetDir);

            // Безопасный расчет поворота
            float dot = XMVectorGetX(XMVector3Dot(data.ForwardEn, targetDir));
            if (fabsf(dot) < 0.9999f) { // Если не параллельны
                XMVECTOR axis = XMVector3Cross(data.ForwardEn, targetDir);
                axis = XMVector3Normalize(axis);
                float angle = acosf(dot);
                XMVECTOR rotQuat = XMQuaternionRotationAxis(axis, angle);

                data.currentRotation = XMQuaternionMultiply(data.currentRotation, rotQuat);
                data.currentRotation = XMQuaternionNormalize(data.currentRotation);

                // Обновляем базовые векторы
                data.ForwardEn = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), data.currentRotation);
                data.UpEn = XMVector3Rotate(data.defaultUp, data.currentRotation);
                data.RightEn = XMVector3Cross(data.UpEn, data.ForwardEn);

            }
        }
    }
        //UpdateRotation(direction);
        
    

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

        //UpdateRotation(direction);
        drawString("chasing player", window.width / 3, window.height / 3, 1.f, true);
    }

}

Enemy::EnemyAI enemyAI(enemyData);

void updateEnemyPosition(float deltaTime, point3d& heroPosition, point3d& enemyPositions) {
    enemyAI.AiUpdate(deltaTime, heroPosition, enemyPositions);
    
}