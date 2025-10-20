

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

        data.playerPosHistory.push_back(heroPosition);
        if (data.playerPosHistory.size() > 30) {
            data.playerPosHistory.erase(data.playerPosHistory.begin());
        }

        data.playerDistance = (heroPosition - enemyPositions).magnitude();
        data.playerVisible = (data.playerDistance < 40000.0f);

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
            if (data.isPreparingAttack) {
               
                data.prepareAttackTimer -= deltaTime;

                
                point3d toTarget = data.delayedAttackTarget - enemyPositions;
                UpdateRotation(toTarget.normalized());

                // Когда подготовка завершена - начинаем атаку
                if (data.prepareAttackTimer <= 0.0f) {
                    data.isPreparingAttack = false;
                    data.attackTimer = 1500.0f; // Длительность полета
                }
            }
            else {
                // Фаза полета к цели
                AttackPlayer(deltaTime, heroPosition, enemyPositions, player);
                if (data.attackTimer <= 0.0f) {
                    data.currentState = AIState::ORBIT;
                    data.attackCooldown = 4000.0f;
                    data.isPreparingAttack = false;
                }
            }
            break;
        case AIState::JUMP_ATTACK:
            JumpAttack(deltaTime, heroPosition, enemyPositions, player);
            if (data.attackTimer <= 0.0f) {
                data.currentState = AIState::ORBIT;
                data.attackCooldown = 5000.0f;
                data.isJumping = false;
               
            }
            break;

        case AIState::BOOM_ATTACK:
            Explosion( deltaTime, enemyPositions, player);
            if (data.attackTimer <= 0.0f) {
                data.currentState = AIState::ORBIT;
                data.attackCooldown = 5000.0f;

            }
            break;
        }

    

        // Обновляем таймеры
        data.attackCooldown -= deltaTime;
        data.attackTimer -= deltaTime;
    }

    void EnemyAI::DamageSound() {
        srand(time(0));
        float Cos = rand() % 8;

        if (Cos == 0) {
            ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\damage1.wav");
        }
        else if (Cos == 1) {
            ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\damage2.wav");
        }
        else if (Cos == 2) {
            ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\damage3.wav");
        }
        else if (Cos == 3) {
            ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\damage4.wav");
        }
        else if (Cos == 4) {
            ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\damage5.wav");
        }
        else if (Cos == 5) {
            ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\damage6.wav");
        }
        else if (Cos == 6) {
            ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\damage7.wav");
        }
        else if (Cos == 7) {
            ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\damage8.wav");
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

        if (moveDir.magnitude() > 0.1f) {
            UpdateRotation(moveDir.normalized());
        }
        data.enemyConstellationOffset = XMMatrixRotationQuaternion(data.currentRotation) *
            XMMatrixTranslation(enemyPositions.x, enemyPositions.y, enemyPositions.z);

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
        if (data.attackCooldown <= 0.0f) {
            // Генерируем случайное число от 0 до 100
            int attackType = rand() % 100;

            if (attackType < 50) { // Обычная атака с подготовкой
                // Берем позицию игрока 0.5 секунд назад (примерно 15 кадров назад)
                if (data.playerPosHistory.size() > 15) {
                    data.delayedAttackTarget = data.playerPosHistory[data.playerPosHistory.size() - 16];
                }
                else {
                    data.delayedAttackTarget = heroPos; // Если истории нет - текущая позиция
                }

                AttakDir = data.delayedAttackTarget - enemyPos;
                data.currentState = AIState::ATTACK;
                data.isPreparingAttack = true;
                data.prepareAttackTimer = 800.0f; // 0.8 секунд подготовки
                data.attackTimer = 2300.0f; // Общее время атаки
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
                data.shouldStartAccumulation = true; // НОВЫЙ ФЛАГ
                data.accumulationPos = enemyPos; // Сохраняем позицию
            }
        }
    }

    void EnemyAI::AttackPlayer(float deltaTime, point3d& heroPos, point3d& enemyPos, float& player) {
        if (data.attackTimer <= 0.0f) return;

        // Летим к запомненной позиции (delayedAttackTarget)
        point3d attackDir = (data.delayedAttackTarget - enemyPos).normalized();

        float moveDistance = data.chaseSpeed * 12.0f * deltaTime;
        enemyPos += attackDir * moveDistance;

        // Проверяем столкновение с реальным игроком
        float distanceToPlayer = (heroPos - enemyPos).magnitude();

        if (distanceToPlayer < 3000.0f) {
            bool attackBlocked = (current_weapon == weapon_name::Shield && energy >= energyCost.shieldBlock);

            if (!attackBlocked) {
                data.isAttacking = true;
                player -= data.DAMAGE_AI;
            }
            else {
                player -= 0.1f;
                energy -= 100.f;
            }
            DamageSound();
            data.attackTimer = 0.0f;
        }

        UpdateRotation(attackDir);
        data.enemyConstellationOffset = XMMatrixRotationQuaternion(data.currentRotation) *
            XMMatrixTranslation(enemyPos.x, enemyPos.y, enemyPos.z);
    }


    void EnemyAI::JumpAttack(float deltaTime, point3d& heroPos, point3d& enemyPos, float& player) {
        // Фаза прыжка вверх
        if (!data.isShockwaveActive && data.jumpHeight < data.maxJumpHeight) {
            data.jumpHeight += data.jumpSpeed * deltaTime;
            enemyPos.y -= data.jumpSpeed * deltaTime; // Обратите внимание: это УМЕНЬШАЕТ Y - возможно должно быть enemyPos.y +=

            // Поворачиваем врага вверх во время прыжка
            XMVECTOR upDir = XMVectorSet(0, 1, 0, 0);
            data.currentRotation = XMQuaternionRotationAxis(data.RightEn, XM_PI / 4);
        }
        else if (data.isShockwaveActive == false) {
            data.jumpHeight -= data.jumpSpeed * deltaTime;
            enemyPos.y += data.jumpSpeed * deltaTime; // Обратите внимание: это УВЕЛИЧИВАЕТ Y
            

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
        // Фаза ударной волны - УЛУЧШЕННАЯ ПРОВЕРКА
        else if (data.isShockwaveActive) {
            data.shockwaveRadius += data.shockwaveSpeed * deltaTime;

            // Центр ударной волны - на земле под врагом
            point3d shockwaveCenter = point3d(enemyPos.x, data.lastOrbitPosition.y, enemyPos.z);

            // Проверяем расстояние от центра волны до игрока
            float distanceToShockwave = (heroPos - shockwaveCenter).magnitude();
            bool isPlayerInRange = (distanceToShockwave <= data.shockwaveRadius*1.5f);


            if (isPlayerInRange && !data.damageApplied) {
                bool attackBlocked = (current_weapon == weapon_name::Shield && energy >= energyCost.shieldBlock);

                if (!attackBlocked) {
                    data.isAttacking = true;
                    player -= data.DAMAGE_AI * 2.f;
                    data.damageApplied = true;
                    
                   

                }
                else {
                    player -= 0.1f;
                    energy -= 200.f;
                }
                DamageSound();
            }

            // Завершаем атаку, когда волна достигла максимума
            if (data.shockwaveRadius >= data.maxShockwaveRadius) {
                data.isShockwaveActive = false;
                data.attackTimer = 0.0f;
                data.damageApplied = false;
            }
        }

        // Обновляем матрицу трансформации
        data.enemyConstellationOffset = XMMatrixRotationQuaternion(data.currentRotation) *
            XMMatrixTranslation(enemyPos.x, enemyPos.y, enemyPos.z);
    }

    void EnemyAI::Explosion(float deltaTime, point3d& enemyPos , float& player) {
        // Фаза подготовки взрыва
        if (!data.isBoomExploding) {
            data.isBoomPreparing = true;
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
                data.isBoomPreparing = false;
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

                bool attackBlocked = (current_weapon == weapon_name::Shield && energy >= energyCost.shieldBlock);

                float distance = (heroPos - enemyPos).magnitude();
                if (distance < data.boomRadius + 3000.0f) {
                    if (!attackBlocked) {
                        data.isAttacking = true;
                        player -= data.DAMAGE_AI * 3.1f;
                    }
                    else {
                        player -= 0.1f;
                        energy -= 250.f;

                    }
                }
                data.isBoomExploding = false;
                data.isBoomPreparing = false;
                data.boomCurrentTime = 0.0f;
                data.boomRadius = 0.0f;
                data.attackTimer = 0.0f;
                DamageSound();
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