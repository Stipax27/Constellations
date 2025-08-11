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
        data.playerVisible = (data.playerDistance < 20000.0f);

        switch (data.currentState) {
        case AIState::PATROL:
            Patrol(deltaTime, enemyPositions);
            if (data.playerVisible) {
                data.currentState = AIState::ORBIT;
                data.attackCooldown = 2000.0f; 
            }
            break;

        case AIState::ORBIT:
            OrbitPlayer(deltaTime, heroPosition, enemyPositions);
            if (!data.playerVisible) {
                data.currentState = AIState::PATROL;
                splineInitialized = false;
            }
            else if (data.attackCooldown <= 0.0f) {
                data.currentState = AIState::ATTACK;
                data.attackTimer = 500.f; // Длительность атаки 0.5 секунды
                data.lastOrbitPosition = enemyPositions; // Запоминаем позицию перед атакой
            }
            break;

        case AIState::ATTACK:
            AttackPlayer(deltaTime, heroPosition, enemyPositions);
            if (data.attackTimer <= 0.0f) {
                data.currentState = AIState::ORBIT;
                data.attackCooldown = 2000.0f; // Следующая атака через 2 секунды
            }
            break;
        }

        // Обновляем таймеры
        data.attackCooldown -= deltaTime;
        data.attackTimer -= deltaTime;
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

    void EnemyAI::OrbitPlayer(float deltaTime, point3d& heroPos, point3d& enemyPos) {
        // Вычисляем направление к игроку
        point3d toPlayer = heroPos - enemyPos;
        float currentDist = toPlayer.magnitude();
        point3d dirToPlayer = toPlayer.normalized();

        // Вычисляем тангенциальное направление для орбитального движения
        point3d tangentDir = point3d(-dirToPlayer.z, 0.0f, dirToPlayer.x).normalized();

        // Корректируем расстояние до игрока
        float distanceCorrection = 0.0f;
        if (currentDist > data.orbitRadius * 1.1f) {
            distanceCorrection = 0.5f; // Приближаемся
        }
        else if (currentDist < data.orbitRadius * 0.9f) {
            distanceCorrection = -0.5f; // Отдаляемся
        }

        // Комбинированное движение: по орбите + коррекция расстояния
        point3d moveDir = (tangentDir * data.orbitSpeed * deltaTime * 50000.0f) +
            (dirToPlayer * distanceCorrection * data.orbitSpeed * deltaTime * 50000.0f);

        enemyPos += moveDir;
        data.enemyConstellationOffset = XMMatrixRotationQuaternion(data.currentRotation) *
            XMMatrixTranslation(enemyPos.x, enemyPos.y, enemyPos.z);

        UpdateRotation(dirToPlayer);
    }

    void EnemyAI::AttackPlayer(float deltaTime, point3d& heroPos, point3d& enemyPos) {
        // Быстро летим к игроку
        point3d attackDir = (heroPos - enemyPos).normalized();
        enemyPos += attackDir * data.chaseSpeed * 5.0f * deltaTime;

        // Обновляем матрицу трансформации
        data.enemyConstellationOffset = XMMatrixRotationQuaternion(data.currentRotation) *
            XMMatrixTranslation(enemyPos.x, enemyPos.y, enemyPos.z);

        UpdateRotation(attackDir);

        // Проверяем столкновение с игроком
        if ((heroPos - enemyPos).magnitude() < 0.0f) {
            // Наносим урон игроку (10 единиц за атаку)
            // player.TakeDamage(10.0f);

            // Возвращаемся на орбиту досрочно, если достигли игрока
            data.attackTimer = 0.0f;
        }
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