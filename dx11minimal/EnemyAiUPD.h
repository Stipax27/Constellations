namespace Enemy {
    
    void SplinePath::BuildSpline(const std::vector<point3d>& waypoints) {
        segments.clear();
        totalLength = 0.0f;

        if (waypoints.size() < 2) return;

        for (size_t i = 0; i < waypoints.size(); ++i) {
            SplineSegment seg;

            point3d p0 = waypoints[(i - 1 + waypoints.size()) % waypoints.size()];
            point3d p1 = waypoints[i];
            point3d p2 = waypoints[(i + 1) % waypoints.size()];
            point3d p3 = waypoints[(i + 2) % waypoints.size()];

            seg.p0 = p1;
            seg.p1 = p1 + (p2 - p0) * 0.2f;
            seg.p2 = p2 - (p3 - p1) * 0.2f;
            seg.p3 = p2;

            seg.length = (p2 - p1).magnitude();
            totalLength += seg.length;

            segments.push_back(seg);
        }
    }

    point3d SplinePath::Evaluate(float t) const {
        if (segments.empty()) return point3d();

        float normalizedT = t * totalLength;
        float accumulated = 0.0f;

        for (const auto& seg : segments) {
            if (normalizedT <= accumulated + seg.length) {
                float localT = (normalizedT - accumulated) / seg.length;
                return CubicBezier(seg.p0, seg.p1, seg.p2, seg.p3, localT);
            }
            accumulated += seg.length;
        }

        return segments.back().p3;
    }

    point3d SplinePath::CubicBezier(point3d p0, point3d p1, point3d p2, point3d p3, float t) const {
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * t;

        point3d p = uuu * p0;
        p += 3 * uu * t * p1;
        p += 3 * u * tt * p2;
        p += ttt * p3;

        return p;
    }

 
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

            /*case AIState::CHASE:
                Chase(heroPosition, enemyPositions, deltaTime);
                if (!data.playerVisible) {
                    data.currentState = AIState::PATROL;
                    splineInitialized = false;
                }
                break;*/
        }
    }

    void EnemyAI::Patrol(float deltaTime, point3d& enemyPositions) {
        if (data.waypoints.empty()) return;

        if (!splineInitialized) {
            splinePath.BuildSpline(data.waypoints);
            splineInitialized = true;
            splineProgress = 0.0f;
        }

        splineProgress += data.patrolSpeed * deltaTime / splinePath.totalLength;
        if (splineProgress >= 1.0f) {
            splineProgress = 0.0f;
        }

        point3d targetPos = splinePath.Evaluate(splineProgress);
        point3d moveDir = (targetPos - enemyPositions).normalized() * data.patrolSpeed * deltaTime;

        // Обновляем позицию и матрицу трансформации
        enemyPositions += moveDir;

        data.enemyConstellationOffset = XMMatrixRotationQuaternion(data.currentRotation) *
            XMMatrixTranslation(enemyPositions.x, enemyPositions.y, enemyPositions.z);

        if (moveDir.magnitude() > 0.1f) {
            UpdateRotation(moveDir.normalized());
        }
    }

    void EnemyAI::Chase(point3d& heroPos, point3d& enemyPositions, float deltaTime) {
        point3d direction = (heroPos - enemyPositions).normalized();
        point3d moveDir = direction * data.chaseSpeed * (deltaTime / 1000.0f);

        // Обновляем позицию и матрицу трансформации
        enemyPositions += moveDir;
        data.enemyConstellationOffset = XMMatrixRotationQuaternion(data.currentRotation) *
            XMMatrixTranslation(enemyPositions.x, enemyPositions.y, enemyPositions.z);

        UpdateRotation(direction);
    }

    void EnemyAI::UpdateRotation(point3d direction) {
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

                // Обновляем векторы ориентации
                data.ForwardEn = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), data.currentRotation);
                data.UpEn = XMVector3Rotate(data.defaultUp, data.currentRotation);
                data.RightEn = XMVector3Cross(data.UpEn, data.ForwardEn);

                // Обновляем матрицу с новым вращением
                XMVECTOR pos = data.enemyConstellationOffset.r[3];
                data.enemyConstellationOffset = XMMatrixRotationQuaternion(data.currentRotation) *
                    XMMatrixTranslation(XMVectorGetX(pos), XMVectorGetY(pos), XMVectorGetZ(pos));
            }
        }
    }
}

void updateEnemyPosition(float deltaTime, point3d& heroPosition, point3d& enemyPositions) {
    static Enemy::EnemyAI enemyAI;
    enemyAI.AiUpdate(deltaTime, heroPosition, enemyPositions);

    // После обновления позиции получаем актуальную матрицу из EnemyAI
    Enemy::enemyData.enemyConstellationOffset = enemyAI.data.enemyConstellationOffset;
}