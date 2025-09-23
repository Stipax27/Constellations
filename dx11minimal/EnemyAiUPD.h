namespace Enemy {

    point3d AttakDir;

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


    void EnemyAI::AiUpdate(float deltaTime, point3d& heroPosition, point3d& enemyPositions , float& player) {
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
            break;

        case AIState::ATTACK:
            AttackPlayer(deltaTime, heroPosition, enemyPositions, player);
            if (data.attackTimer <= 0.0f) {
                data.currentState = AIState::ORBIT;
                data.attackCooldown = 5000.0f; // Следующая атака через 5 секунды
            }
            break;

        case AIState::JUMP_ATTACK:
            JumpAttack(deltaTime, heroPosition, enemyPositions, player);
            if (data.attackTimer <= 0.0f) {
                data.currentState = AIState::ORBIT;
                data.attackCooldown = 5000.0f;
                data.isJumping = false;
                data.isShockwaveActive = false;
            }
            break;

        case AIState::BOOM_ATTACK:
            Explosion( deltaTime, enemyPositions, player);
            if (data.attackTimer <= 0.0f) {
                data.currentState = AIState::ORBIT;

            }
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
        point3d moveDir = direction * data.chaseSpeed * (deltaTime / 1000.f);

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

        // Корректируем расстояние до игроку
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

        // Проверка на атаку
        if (data.attackCooldown <= 0.0f && !data.isAttacking) {
            // Генерируем случайное число от 0 до 100
            int attackType = rand() % 100;

            if (attackType < 50) { // 50% - обычная атака
                AttakDir = heroPos - enemyPos;
                data.currentState = AIState::ATTACK;
                data.attackTimer = 500.f;
                data.lastOrbitPosition = enemyPos;
                data.attackCooldown = 0.0f;
                
            }
            else if (attackType < 80) { // 30% - атака в прыжке (50-79)
                AttakDir = heroPos - enemyPos;
                data.currentState = AIState::JUMP_ATTACK;
                data.attackTimer = 1500.0f;
                data.lastOrbitPosition = enemyPos;
                data.isJumping = true;
                data.jumpHeight = 0.0f;
                data.attackCooldown = 0.0f;
                
            }
            else { // 20% - взрыв (80-99)
                AttakDir = heroPos - enemyPos;
                data.currentState = AIState::BOOM_ATTACK;
                data.attackTimer = 5000.0f;
                data.lastOrbitPosition = enemyPos;
                data.attackCooldown = 0.0f;
                data.isBoomExploding = true;
                
            }
        }
    }

    void EnemyAI::AttackPlayer(float deltaTime, point3d& heroPos, point3d& enemyPos , float& player) {
        // Быстро летим к игроку
        point3d attackDir = AttakDir.normalized();
        enemyPos += (attackDir * data.chaseSpeed * 5.0f * deltaTime) / data.attackDuration;

        // Обновляем матрицу трансформации
        data.enemyConstellationOffset = XMMatrixRotationQuaternion(data.currentRotation) *
            XMMatrixTranslation(enemyPos.x, enemyPos.y, enemyPos.z);

        UpdateRotation(attackDir);

        // Проверяем столкновение с игроком
        if ((heroPos - enemyPos).magnitude() < 1000.0f) {
            player -= 1.f;
            data.isAttacking = true;
            
            // Возвращаемся на орбиту досрочно, если достигли игрока
            //data.attackTimer = 0.0f;
        }

    }

    void EnemyAI::JumpAttack(float deltaTime, point3d& heroPos, point3d& enemyPos , float& player) {
        // Фаза прыжка вверх
        if (!data.isShockwaveActive && data.jumpHeight < data.maxJumpHeight) {
            data.jumpHeight += data.jumpSpeed * deltaTime;
            enemyPos.y -= data.jumpSpeed * deltaTime;

            // Поворачиваем врага вверх во время прыжка
            XMVECTOR upDir = XMVectorSet(0, 1, 0, 0);
            data.currentRotation = XMQuaternionRotationAxis(data.RightEn, XM_PI / 4);
        }
        // Фаза падения и создания ударной волны
        else if (!data.isShockwaveActive) {
            data.jumpHeight -= data.jumpSpeed * deltaTime;
            enemyPos.y += data.jumpSpeed * deltaTime;

            if (enemyPos.y <= data.lastOrbitPosition.y) {
                enemyPos.y = data.lastOrbitPosition.y;
                data.isShockwaveActive = true;
                data.shockwaveStartTime = currentTime;
                data.shockwaveRadius = 0.0f;

                // Восстанавливаем нормальную ориентацию
                point3d toPlayer = heroPos - enemyPos;
                UpdateRotation(toPlayer.normalized());
            }
        }
        // Фаза ударной волны
        else if (data.isShockwaveActive) {
            data.shockwaveRadius += data.shockwaveSpeed * deltaTime;

            // Проверяем попадание по игроку
            if ((heroPos - enemyPos).magnitude() < data.shockwaveRadius) {
                player -= 2.f;
                data.isAttacking = true;
            }

            // Завершаем атаку, когда волна достигла максимума
            if (data.shockwaveRadius >= data.maxShockwaveRadius) {
                data.isShockwaveActive = false;
                data.attackTimer = 0.0f;
            }
        }

        // Обновляем матрицу трансформации
        data.enemyConstellationOffset = XMMatrixRotationQuaternion(data.currentRotation) *
            XMMatrixTranslation(enemyPos.x, enemyPos.y, enemyPos.z);
    }

    void EnemyAI::Explosion(float deltaTime, point3d& enemyPos , float& player) {
        // Фаза подготовки взрыва
        if (!data.isBoomExploding) {
            data.boomCurrentTime += deltaTime;

            // Визуальные эффекты подготовки (пульсация, изменение цвета)
            float pulse = 0.5f + 0.5f * sinf(data.boomCurrentTime * 0.01f);
            float scale = 1.0f + pulse * 0.2f;

            // Применяем масштабирование к врагу
            XMMATRIX scaleMat = XMMatrixScaling(scale, scale, scale);
            data.enemyConstellationOffset = XMMatrixRotationQuaternion(data.currentRotation) *
                scaleMat *
                XMMatrixTranslation(enemyPos.x, enemyPos.y, enemyPos.z);

            // Когда подготовка завершена, начинаем взрыв
            if (data.boomCurrentTime >= data.boomPrepareTime) {
                data.isBoomExploding = true;
                data.boomStartTime = currentTime;
            }
        }
        // Фаза взрыва
        else {
            // Увеличиваем радиус взрыва
            data.boomRadius += (data.maxBoomRadius * (deltaTime / 1000.0f))/5.f;

            // Создаем визуальные эффекты взрыва (можно добавить частицы, свечение)
            //CreateExplosionEffects(enemyPos, data.boomRadius);

            // Проверяем попадание по игроку
            point3d heroPos = point3d(
                XMVectorGetX(Hero::state.constellationOffset.r[3]),
                XMVectorGetY(Hero::state.constellationOffset.r[3]),
                XMVectorGetZ(Hero::state.constellationOffset.r[3])
            );

            

            // Завершаем атаку, когда взрыв достиг максимума
            if (data.boomRadius >= data.maxBoomRadius) {

                float distance = (heroPos - enemyPos).magnitude();
                if (distance < data.boomRadius + 3000.0f) {
                    player -= 5.f;
                    data.isAttacking = true;
                }
                data.isBoomExploding = false;
                data.isBoomPreparing = false;
                data.boomCurrentTime = 0.0f;
                data.boomRadius = 0.0f;
                data.attackTimer = 0.0f;

                // Восстанавливаем нормальный масштаб
                data.enemyConstellationOffset = XMMatrixRotationQuaternion(data.currentRotation) *
                    XMMatrixTranslation(enemyPos.x, enemyPos.y, enemyPos.z);
            }
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

static Enemy::EnemyAI enemyAI;
void updateEnemyPosition(float deltaTime, point3d& heroPosition, point3d& enemyPositions , float& player) {
    enemyAI.AiUpdate(deltaTime, heroPosition, enemyPositions , player);

    // После обновления позиции получаем актуальную матрицу из EnemyAI
    Enemy::enemyData.enemyConstellationOffset = enemyAI.data.enemyConstellationOffset;
}