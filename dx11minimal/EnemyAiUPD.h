namespace Enemy
{
    void EnemyAI::AiUpdate(float deltaTime, const point3d& heroPosition) {
       
        switch (data.currentState) {
        case AIState::PATROL:
            Patrol(deltaTime);
            if (data.playerVisible) data.currentState = AIState::CHASE;
            break;

        case AIState::CHASE:
            Chase(heroPosition);
            if (data.playerDistance < 500.0f) {
                data.currentState = AIState::ATTACK;
            }
            else if (!data.playerVisible) {
                data.currentState = AIState::SEARCH;
                data.waypoints.push_back(GetPosition());
            }
            break;
        }
    }

    point3d EnemyAI::GetPosition() const {
        return point3d(
            data.enemyConstellationOffset.r[3].m128_f32[0],
            data.enemyConstellationOffset.r[3].m128_f32[1],
            data.enemyConstellationOffset.r[3].m128_f32[2]
        );
    }

    void EnemyAI::Patrol(float deltaTime) {
        if (data.waypoints.empty()) return;

        point3d currentPos = GetPosition();
        point3d& target = data.waypoints[data.currentWaypoint];
        point3d direction = target - currentPos;
        float distance = direction.magnitude();

        if (distance < 20.0f) {
            data.currentWaypoint = (data.currentWaypoint + 1) % data.waypoints.size();
            return;
        }

        point3d moveDir = direction.normalized() * data.patrolSpeed * deltaTime;
        data.enemyConstellationOffset = XMMatrixMultiply(
            data.enemyConstellationOffset,
            XMMatrixTranslation(moveDir.x, moveDir.y, moveDir.z)
        );

        UpdateRotation(direction);
    }

    void EnemyAI::Chase(const point3d& heroPos) {
        point3d currentPos = GetPosition();
        point3d direction = (heroPos - currentPos).normalized();
        data.currentDirection = direction;

        data.enemyConstellationOffset = XMMatrixMultiply(
            data.enemyConstellationOffset,
            XMMatrixTranslation(
                direction.x * data.chaseSpeed,
                direction.y * data.chaseSpeed,
                direction.z * data.chaseSpeed)
        );

        UpdateRotation(direction);
        drawString("chasing player", window.width / 3, window.height / 3, 1.f, true);
    }

    void EnemyAI::UpdateRotation(const point3d& direction) {
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

// √лобальный экземпл€р AI
Enemy::EnemyAI enemyAI(enemyData);

void updateEnemyPosition(float deltaTime, const point3d& heroPosition) {
    enemyAI.AiUpdate(deltaTime, heroPosition);
    enemyPositions = enemyData.enemyConstellationOffset.r[3];
}
