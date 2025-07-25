bool t = true;
point3d attackDirection;

namespace drawer
{
    int GetRandom(int min = 1, int max = 0)
    {
        if (max > min)
        {
            return rand() % (max - min) + min;
        }
        else
        {
            return rand() % min;
        }
    }

    
    void drawLine(point3d& p1, point3d& p2 , float sz = 2.f )
    {
        point3d vector = p2 - p1;
        point3d mid = p1.lerp(p2, 0.5f);

        

        ConstBuf::ConstToVertex(4);
        ConstBuf::ConstToPixel(4);


        ConstBuf::global[0] = XMFLOAT4(p1.x, p1.y, p1.z, sz);
        ConstBuf::global[1] = XMFLOAT4(p2.x, p2.y, p2.z, sz);

        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToVertex(5);

        Draw::elipse(1);
    }

    void drawLinks(Constellation& Constellation,float sz)
    {
        std::vector <point3d>& starArray = Constellation.starsCords;
        std::vector<std::vector<float>>& starEdges = Constellation.constellationEdges;
        std::vector <float>& starHealth = Constellation.starsHealth;

        int starsEdgesCount = starEdges.size();
        for (int i = 0; i < starsEdgesCount; i++)
        {
            point3d point1 = TransformPoint(starArray[starEdges[i][0]], Constellation.Transform);
            point3d point2 = TransformPoint(starArray[starEdges[i][1]], Constellation.Transform);

            if (starHealth[starEdges[i][0]] > 0 && starHealth[starEdges[i][1]] > 0) // - Стало
            {
                drawLine(point1, point2,sz);
            }
        }
    }

    void drawStarPulse(Constellation& Constellation, bool colorOverride = false, float finalStarRad = 10.f )
    {
        std::vector <point3d>& starArray = Constellation.starsCords;
        std::vector <float>& starHealth = Constellation.starsHealth;

        int starsCount = starArray.size();

        for (int i = 0; i < starsCount; i++)
        {
            point3d point;
            point = starArray[i];
            point3d worldPoint = TransformPoint(point, Constellation.Transform);

            XMVECTOR p = XMVECTOR{ worldPoint.x,worldPoint.y,worldPoint.z,1 };
            p = XMVector4Transform(p, XMMatrixTranspose(ConstBuf::camera.view[0]) * XMMatrixTranspose(ConstBuf::camera.proj[0]));

            float px = .5f * XMVectorGetX(p) / XMVectorGetW(p) + .5f;
            float py = -.5f * XMVectorGetY(p) / XMVectorGetW(p) + .5f;

            point3d screenPoint = { px,py,0 };
            screenPoint.x *=window.width;
            screenPoint.y *= window.height;

            // Пульсирование Звёзд при наведение мыши.
            
            /*if (uiFunc)
            {
                uiFunc(screenPoint, Constellation, i);
            }*/

            
            if (finalStarRad > 0)
            {
                point.draw(worldPoint, finalStarRad);
            }
        }
    }

    void drawСonstellation(Constellation& Constellation, bool colorOverride = false , float finalStarRad = 10.f, float sz =2.f)
    {
        Shaders::vShader(1);
        Shaders::pShader(1);

        drawStarPulse(Constellation, colorOverride, finalStarRad);

        Shaders::vShader(4);
        Shaders::pShader(4);

        drawLinks(Constellation,sz);
    }

    void drawStarField()
    {
        Shaders::vShader(2);
        Shaders::pShader(2);
        Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
        Draw::Starfield(1);
    }

    void drawStars()
    {
        Shaders::vShader(3);
        Shaders::pShader(3);
        Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
        Draw::Starfield(1);
    }

    void drawGalaxyFog(int shaderID)
    {
        Shaders::vShader(shaderID);
        Shaders::pShader(shaderID);
        Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
        Draw::GalaxyFog(5000000);
    }

    void drawCursor(int shaderID)
    {
        Shaders::vShader(shaderID);
        Shaders::pShader(shaderID);
        Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);

        ConstBuf::global[0] = XMFLOAT4(mouse.pos.x / width * 2 - 1, -(mouse.pos.y / height * 2 - 1), 0.0f, 1.0f);
        Draw::Cursor();
    }

    const COLORREF colors[] =
    {
        RGB(255, 0, 0),    // Красный
        RGB(255, 165, 0),  // Оранжевый
        RGB(255, 255, 0),  // Желтый
        RGB(0, 255, 0),    // Зеленый
        RGB(0, 255, 255),  // Голубой
        RGB(0, 0, 255),    // Синий
        RGB(128, 0, 128)   // Фиолетовый
    };



    void morphWepon(std::vector <point3d>& starArray1, std::vector<std::vector<float>> starEdges1, std::vector <point3d>& starArray2, std::vector<std::vector<float>> starEdges2, std::vector <point3d>& morphArray, std::vector <std::vector <float>> Morp_indices, std::vector <float> Morp_health)
    {
        morphArray.clear();
        Morp_indices.clear();
        Morp_health.clear();
        int sz1 = starArray1.size();
        int sz2 = starArray2.size();
        int sz3 = starEdges1.size();
        int sz4 = starEdges2.size();
        if (sz1 < sz2)
        {
            for (int i = 0; i < sz1;i++)
            {
                float morphSpeed = 0.01;
                morphArray.push_back(point3d::lerp(starArray1[i], starArray2[i], (0.5 + 0.5 * sin(currentTime * morphSpeed))));
            }
        }
        else
        {
            for (int i = 0; i < sz2;i++)
            {
                float morphSpeed = 0.01;
                morphArray.push_back(point3d::lerp(starArray1[i], starArray2[i], (0.5 + 0.5 * sin(currentTime * morphSpeed))));
            }
        }
        if (sz3 > sz4)
        {
            for (float i = 0; i < sz4;i++)
            {
                Morp_indices.push_back({ i, i + 1 });
            }
        }
        else
        {
            for (float i = 0; i < sz3;i++)
            {
                Morp_indices.push_back({ i, i + 1 });
            }
        }
        for (int i = 0; i < 15;i++)
        {
            Morp_health.push_back(1);
        }

        //drawСonstellation(morphArray, Morp_indices, Morp_health); Отключено
    }

    void drawPlayerСonstellationToMenu()
    {
        Shaders::vShader(1);
        Shaders::pShader(1);
        startTime = currentTime;
        int n = (currentTime / 1000) % starSet.size();
        uiFunc = &menuUI;
        //linksDivider = 15;
        if (gameState == gameState_::confirmSign) n = player_sign;
        Constellation& c = *starSet[n];
        float t = currentTime * 0.001;
        c.Transform = XMMatrixScaling(120, 120, 120) * XMMatrixRotationY(t) * XMMatrixTranslation(0, 0, 500);
        drawСonstellation(*starSet[player_sign]);
    }

   
    void drawSwordLine(float CenterX, float CenterY)
    {
        float Length = 100;

        modelProject = &NullProject;


        point3d Sword1, Sword2;

        Sword1.x = CenterX + Length;
        Sword1.y = CenterY + Length;
        Sword1.z = 0;

        Sword2.x = CenterX - Length;
        Sword2.y = CenterY - Length;
        Sword2.z = 0;

        Shaders::vShader(4);
        Shaders::pShader(4);

        drawLine(Sword1, Sword2);

    }

    void drawShieldCircle(float CenterX, float CenterY,float CenterZ)
    {
        float shieldRadius = 100;

        // Центр = oldmouse (круг растёт из этой точки)
        float centerX = CenterX;
        float centerY = CenterY;
        modelProject = &NullProject;

        for (int i = 0; i < 36; i++) {
            float angle = i * (2 * PI / 36);  // 36 точек для гладкого круга
            float nextAngle = (i + 1) * (2 * PI / 36);

            point3d shield1, shield2;

            shield1.x = centerX + shieldRadius * cos(angle);
            shield1.y = centerY + shieldRadius * sin(angle);
            shield1.z = CenterZ;

            shield2.x = centerX + shieldRadius * cos(nextAngle);
            shield2.y = centerY + shieldRadius * sin(nextAngle);
            shield2.z = CenterZ;


            drawLine(shield1, shield2);


        }
    }

    void drawBowLine(float CenterX, float CenterY)
    {
        float shieldRadius = 20;

        // Центр = oldmouse (круг растёт из этой точки)
        float centerX = CenterX;
        float centerY = CenterY;
        modelProject = &NullProject;

        for (int i = 0; i < 36; i++) {
            float angle = i * (2 * PI / 36);  // 36 точек для гладкого круга
            float nextAngle = (i + 1) * (2 * PI / 36);

            point3d shield1, shield2;

            shield1.x = centerX + shieldRadius * cos(angle);
            shield1.y = centerY + shieldRadius * sin(angle);
            shield1.z = 0;

            shield2.x = centerX + shieldRadius * cos(nextAngle);
            shield2.y = centerY + shieldRadius * sin(nextAngle);
            shield2.z = 0;


            drawLine(shield1, shield2);


        }
    }
    void drawArrow(float CenterX, float CenterY)
    {
        float Length = 50;

        float centerX = CenterX;
        float centerY = CenterY;
        modelProject = &NullProject;

        point3d Arrow1, Arrow2;

        Arrow1.x = centerX + Length;
        Arrow1.y = centerY + Length;
        Arrow1.z = 0;

        Arrow2.x = centerX;
        Arrow2.y = centerY;
        Arrow2.z = 0;

        drawLine(Arrow1, Arrow2);

    }



   


    

    void DrawStarsHP(HDC hdc) {

        for (size_t i = 0; i < starSet[currentEnemyID]->starsCords.size(); ++i) {
            auto pos = starSet[currentEnemyID]->starsCords[i];
            float hp = starSet[currentEnemyID]->starsHealth[i];

            modelTransform(pos, *starSet[currentEnemyID]);
            modelProject(pos);

            if (hp > 0)
            {
                std::string hpText = "HP: " + std::to_string(hp);
                drawString(hpText.c_str(), pos.x, pos.y - 40, .5, true, false, 7);
            }
        }
    }
    void DrawHpEnemyBar()
    {
        auto enemy_const = *starSet[currentEnemyID];
        auto maxHP = enemy_const.maxHP;
        auto progress = getConstellationHP(*starSet[currentEnemyID]) / maxHP;
        auto progressText = "HP: " + std::to_string(progress);

        //linksDivider = 15;
        modelTransform = &placeConstToWorld;
        uiFunc = starIntersectUI;
        modelProject = &fightProject;

        // Параметры временного бара
        const float barWidth = 300;
        const float barHeight = 60;
        const float barX = 0;
        const float barY = .3;
        const float starCount = 10;
        const float starSize = 5.0f;

        point3d Bar = { barX, barY,0 };

        modelTransform(Bar, *starSet[currentEnemyID]);
        modelProject(Bar);

        // Создаем точки для рамки
        point3d topLeft = { Bar.x - barWidth / 2, Bar.y, 0 };
        point3d topRight = { Bar.x + barWidth / 2, Bar.y, 0 };
        point3d bottomLeft = { Bar.x - barWidth / 2, Bar.y + barHeight, 0 };
        point3d bottomRight = { Bar.x + barWidth / 2, Bar.y + barHeight, 0 };

        modelProject = NULL;
        // Рисуем рамку
        drawLine(topLeft, topRight);    // Верх
        drawLine(bottomLeft, bottomRight); // Низ
        drawLine(topLeft, bottomLeft);  // Лево
        drawLine(bottomRight, topRight); // Право

        // Рисуем заполнение из звёзд
        int activeStars = (int)(starCount * progress);
        float starSpacing = barWidth / (float)(starCount + 1);

        for (int i = 0; i < starCount; i++) {
            point3d star = {
                Bar.x + (i + 1) * starSpacing - barWidth / 2,
                Bar.y + barHeight / 2,
                Bar.z
            };

            float currentSize = (i < activeStars) ? starSize * 1.5f : starSize * .0;

            if (i < activeStars) {
                float pulse = 0.5f + 0.5f * sinf(currentTime * 0.005f);
                currentSize += pulse * 2.0f;
            }

            star.draw(star, currentSize);
        }

        // Соединяем звёзды
        for (int i = 0; i < starCount - 1; i++) {
            point3d star1 = {
                Bar.x + (i + 1) * starSpacing - barWidth / 2,
                Bar.y + barHeight / 2,
                Bar.z
            };

            point3d star2 = {
                Bar.x + (i + 2) * starSpacing - barWidth / 2,
                Bar.y + barHeight / 2,
                Bar.z
            };

            drawLine(star1, star2);
        }
        modelProject = &fightProject;
        point3d p = { 0,.3,0 };
        modelTransform(p, enemy_const);
        modelProject(p);
        drawString(progressText.c_str(), p.x, p.y - 50, 1, true);

    }

    void DrawHpHeroBar()
    {
        auto player_const = *starSet[player_sign];
        auto maxHP = player_const.maxHP;
        auto progress = getConstellationHP(*starSet[player_sign]) / maxHP;
        auto progressText = "HP: " + std::to_string(progress);

        //linksDivider = 15;
        modelTransform = &HeroUITransform;
        uiFunc = NULL;
        modelProject = &fightProject;

        // Параметры временного бара
        const float barWidth = 4;
        const float barHeight = .25;
        const float barX = -barWidth / 2;
        const float barY = -barHeight / 2 + 2.5;
        const float starCount = 20;
        const float starSize = 5.0f;

        // Создаем точки для рамки
        point3d topLeft = { barX, barY, 0 };
        point3d topRight = { barX + barWidth, barY, 0 };
        point3d bottomLeft = { barX, barY + barHeight, 0 };
        point3d bottomRight = { barX + barWidth, barY + barHeight, 0 };

        modelTransform(topLeft, player_const);
        modelTransform(topRight, player_const);
        modelTransform(bottomLeft, player_const);
        modelTransform(bottomRight, player_const);

        // Рисуем рамку
        drawer::drawLine(topLeft, topRight);    // Верх
        drawer::drawLine(bottomLeft, bottomRight); // Низ
        drawer::drawLine(topLeft, bottomLeft);  // Лево
        drawer::drawLine(bottomRight, topRight); // Право

        // Рисуем заполнение из звёзд
        int activeStars = (int)(starCount * progress);
        float starSpacing = barWidth / (float)(starCount + 1);

        for (int i = 0; i < starCount; i++) {
            point3d star = {
                barX + (i + 1) * starSpacing,
                barY + barHeight / 2,
                0
            };

            float currentSize = (i < activeStars) ? starSize * 1.5f : starSize * .0;

            if (i < activeStars) {
                float pulse = 0.5f + 0.5f * sinf(currentTime * 0.005f);
                currentSize += pulse * 2.0f;
            }

            modelTransform(star, player_const);
            modelProject(star);

            star.draw(star, currentSize);
        }

        // Соединяем звёзды
        for (int i = 0; i < starCount - 1; i++) {
            point3d star1 = {
                barX + (i + 1) * starSpacing,
                barY + barHeight / 2,
                0
            };

            point3d star2 = {
                barX + (i + 2) * starSpacing,
                barY + barHeight / 2,
                0
            };

            modelTransform(star1, player_const);
            modelTransform(star2, player_const);

            drawer::drawLine(star1, star2);
        }

        point3d p = { 0,2,0 };
        modelTransform(p, player_const);
        modelProject(p);
        drawString(progressText.c_str(), p.x, p.y, 1, true);
    }

    bool isBattleActive = false;
    DWORD battleStartTime;


    void UpdateGame() {
        static const DWORD MAX_BATTLE_TIME = 4 * 60 * 1000;
        static const DWORD MAX_REWIND = 30 * 1000;
        static DWORD battleTime = 60 * 5 * 1000;
        static DWORD timeModifier = 0;
        static DWORD lastInputTime = 0;
        const DWORD inputRepeatDelay = 100;

        if (GetAsyncKeyState('Q')) {
            if (currentTime - lastInputTime > inputRepeatDelay) {
                lastInputTime = currentTime;
                if (battleStartTime + battleTime + timeModifier + 1000 - currentTime <= MAX_BATTLE_TIME) {
                    timeModifier += 1000;
                }
            }
        }

        if (GetAsyncKeyState('E') & 0x8000) {
            if (currentStateIndex > 0) {
                RewindOneStepBack();
                isRewind = true;
            }
            else {
                isRewind = false;
            }
        }
        else {
            isRewind = false;
        }

        if (isBattleActive) {
            LONG remainingTime = (LONG)((battleStartTime + battleTime + timeModifier) - currentTime);

            DWORD totalBattleTime = battleTime + timeModifier;

            if (totalBattleTime > MAX_BATTLE_TIME) {
                timeModifier = MAX_BATTLE_TIME - battleTime;
                remainingTime = (LONG)((battleStartTime + MAX_BATTLE_TIME) - currentTime);
            }

            if (remainingTime > 0) {

                DrawHpHeroBar();
                std::string timeStr = "Time: " + std::to_string(remainingTime / 1000);
                drawString(timeStr.c_str(), window.width / 1.1, 45, 1.f, true);

                if (getConstellationHP(*starSet[currentEnemyID]) < 0)
                {
                    timeModifier = 0;
                    isBattleActive = false;
                    gameState = gameState_::WinFight;
                }
            }
            else
            {
                timeModifier = 0;
                isBattleActive = false;
                gameState = gameState_::EndFight;

            }
        }
    }

    bool CheckSwordStarCollision(const point3d& swordStart, const point3d& swordEnd,
        const point3d& starPos, float starRadius) {
        // Вектор направления атаки меча
        point3d swordDir = swordEnd - swordStart;
        float swordLength = swordDir.magnitude();
        swordDir = swordDir.normalized();

        // Вектор от начала атаки к центру звезды
        point3d toStar = starPos - swordStart;

        // Проекция вектора toStar на направление атаки
        float projection = toStar.dot(swordDir);

        // Ближайшая точка на линии атаки к центру звезды
        point3d closestPoint;
        if (projection < 0) {
            closestPoint = swordStart;
        }
        else if (projection > swordLength) {
            closestPoint = swordEnd;
        }
        else {
            closestPoint = swordStart + swordDir * projection;
        }

        // Расстояние от ближайшей точки до центра звезды
        float distance = (closestPoint - starPos).magnitude();

        // Если расстояние меньше радиуса - есть коллизия
        return distance < starRadius;
    }

    struct StarProjectile {
        point3d position;
        point3d direction;  // Индивидуальное направление для каждой звезды
    };

    std::vector<StarProjectile> attackStars; // Теперь храним звёзды с их направлениями

    void UpdateAttackStars(float deltaTime) {
        // Обновляем позиции звёзд по их индивидуальным направлениям
        for (auto& star : attackStars) {
            star.position += star.direction.normalized() * 2.0f * deltaTime;
        }
    }

    void DrawAttackStars() {
        // Отрисовываем все звёзды
        for (StarProjectile& star : attackStars) {
           star.position.draw(star.position, 5.0f);
        }
    }

    bool isAttacking = false;
    static DWORD lastAttackTime = 0;
    const float projectileSpeed = 2.0f;
    point3d mouseRay;
    point3d start;
    void HandleMouseClick() {
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && currentTime - lastAttackTime > 500) {
            lastAttackTime = currentTime;

            // Получаем позицию героя
            XMVECTOR heroPosition = Hero::state.constellationOffset.r[3];
            start = point3d(
                XMVectorGetX(heroPosition),
                XMVectorGetY(heroPosition),
                XMVectorGetZ(heroPosition)
            );

            // Вычисляем направление атаки от камеры к курсору
            point3d camPos = point3d(
                XMVectorGetX(Camera::state.Eye),
                XMVectorGetY(Camera::state.Eye),
                XMVectorGetZ(Camera::state.Eye)
            );

            mouseRay = GetMouseRay(mouse.pos);
            point3d mousePos = camPos + mouseRay * 7500;
            point3d newDirection = (mousePos - start).normalized();

            // Создаём звёзды с этим направлением
            for (int i = 0; i < 20; i++) {
                StarProjectile newStar;
                newStar.position = start;
                newStar.direction = newDirection; // Фиксируем направление при создании

                // Добавляем небольшой разброс позиции
                point3d up = point3d(XMVectorGetX(Camera::state.Up),
                    XMVectorGetY(Camera::state.Up),
                    XMVectorGetZ(Camera::state.Up));
                point3d right = point3d(XMVectorGetX(Camera::state.Right),
                    XMVectorGetY(Camera::state.Right),
                    XMVectorGetZ(Camera::state.Right));

                newStar.position += up * (i * 10 - 100);
                newStar.position += right * (i * 10 - 100);

                attackStars.push_back(newStar);
            }

            isAttacking = true;
            ProcessSound("Sword.wav");
        }
    }

    void UpdateAttack(float deltaTime) {
        if (isAttacking) {
            // Проверяем коллизии только во время атаки
            Constellation& enemy = *starSet[currentEnemyID];

            // Получаем мировые координаты звезд врага
            for (int i = 0; i < enemy.starsCords.size(); i++) {
                if (enemy.starsHealth[i] <= 0) continue; // Пропускаем уничтоженные звезды

                point3d starWorldPos = TransformPoint(enemy.starsCords[i], enemy.Transform);

                // Проверяем коллизию с каждой звездой врага
                for (auto& star : attackStars) {
                    if (CheckSwordStarCollision(start, star.position, starWorldPos, 1000.f)) {
                        // Наносим урон звезде
                        enemy.starsHealth[i] -= 10.f; // Примерное значение урона

                        // Можно добавить эффекты попадания
                        ProcessSound("Damage.wav");

                        // Прерываем проверку для этой звезды
                        break;
                    }
                }
            }

            isAttacking = false;
        }

        UpdateAttackStars(deltaTime);

        // Очистка улетевших звёзд
        attackStars.erase(
            std::remove_if(attackStars.begin(), attackStars.end(),
                [](const StarProjectile& star) {
                    return (star.position - start).magnitude() > 7500.0f;
                }),
            attackStars.end());
    }


    void DrawSwordAttack() {
        if (isAttacking) return;

        Shaders::vShader(1);
        Shaders::pShader(1);
        Blend::Blending(Blend::blendmode::on);
       
        DrawAttackStars();
    }

    


    struct uiParticle
    {
        point3d pos;
        point3d vel;
        DWORD startTime;
        DWORD lifetime;
    };

    vector<uiParticle*> uiParticles = vector<uiParticle*>{};
    bool isPressed = false;
    void CreateCursorParticles()
    {
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        {
           
            if (!isPressed)
            {
                isPressed = true;
                ProcessSound("Mouse_click1.wav");
                point3d mousePos = point3d(mouse.pos.x / width * 2 - 1, -(mouse.pos.y / height * 2 - 1));
                DWORD curTime = timer::GetCounter();

                for (int i = 0; i < 20; i++)
                {
                    uiParticle* particle = new uiParticle;
                    particle->pos = mousePos;
                    particle->vel = point3d(GetRandom(-100, 100), GetRandom(-100, 100)).normalized() * point3d(aspect, 1) * (float)GetRandom(8, 30) / 100.0f * 0.002f;
                    particle->lifetime = GetRandom(500, 1500);
                    particle->startTime = curTime;

                    uiParticles.push_back(particle);
                }
            }
        }
        else
        {
            isPressed = false;
        }
    }

    void DrawUiParticles(float deltaTime)
    {
        Shaders::vShader(12);
        Shaders::pShader(12);
        Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

        int i = 0;
        DWORD curTime = timer::GetCounter();
        while (i < uiParticles.size())
        {
            uiParticle* particle = uiParticles[i];

            if (curTime - particle->startTime < particle->lifetime)
            {
                ConstBuf::global[0] = XMFLOAT4(particle->pos.x, particle->pos.y, 0.0f, 1 - (float)(curTime - particle->startTime) / (float)particle->lifetime);

                ConstBuf::Update(5, ConstBuf::global);
                ConstBuf::ConstToVertex(5);
                ConstBuf::Update(1, ConstBuf::drawerP);
                ConstBuf::ConstToPixel(1);

                context->Draw(6, 0);

                particle->pos += particle->vel * deltaTime;
                particle->vel *= 0.92f;

                i++;
            }
            else
            {
                uiParticles.erase(uiParticles.begin() + i);
            }
        }
    }

    

    void drawWorld(float deltaTime)
    {
        textStyle.color = RGB(0, 191, 255);
        Draw::Clear({ 0.0f, 0.0588f, 0.1176f, 1.0f });
        Draw::ClearDepth();

       //d2dRenderTarget->BeginDraw();
        switch (gameState)
        {
        case gameState_::MainMenu:
            StartMenu();
            break;

        case gameState_::MonthSelection:
            menuMonthprocessing();
            break;

        case gameState_::DaySelection:
            menuMonthprocessing();
            menuDayprocessing();
            break;

        case gameState_::confirmSign:
            Depth::Depth(Depth::depthmode::off);
            Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
            drawPlayerСonstellationToMenu();
            menuMonthprocessing();
            menuDayprocessing();
            menuConfirmationButton();
            break;

        case gameState_::selectEnemy:
        {
            Camera::state.mouse = true;
            Depth::Depth(Depth::depthmode::off);
            Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
            uiFunc = &constSelectUI;

            drawStarField();
            Shaders::vShader(1);
            Shaders::pShader(1);

            //drawGalaxyFog(7);

            Constellation& playerConst = *starSet[player_sign];
            playerConst.Transform = CreateHeroToWorldMatrix(playerConst);


           
            Constellation& c = *starSet[0]; // Используем текущего врага

            c.Transform = CreateEnemyToWorldMatrix(c);
           
            drawСonstellation(c,false,1000.f,100.f);
           

            HandleMouseClick();
            UpdateAttack(deltaTime);
            DrawSwordAttack();

            drawСonstellation(playerConst);

            std::string curentSignstring = zodiacSignToString(player_sign);
            TextOutA(window.context, window.width * 5 / 6, window.height - window.height / 20., curentSignstring.c_str(), curentSignstring.size());

            drawString("Find Constallations and click on him", window.width / 2, (200. / 1440) * window.height, 1, true);
            drawString("Features:\nMouse wheel to zoom in and out", (1700. / 2560) * window.width, (1200. / 1440) * window.height, .7f, false);

            isBattleActive = false;
            break;

           // drawString("X", 0, 0, 1, false);
           // drawString(std::to_string(mouse.pos.y).c_str(), 0, 0, 1, false);

        }
        case gameState_::DialogStruct:
            initContentData();
            renderContent();
            handleInput();

            break;

        case gameState_::Fight:
        {
            
            if (t) {
                t = false;
                Camera::state.camDist = 100;
            }
            Camera::state.mouse = true;
            Depth::Depth(Depth::depthmode::off);
            SelectWeapon();
            SelectElement();

            DrawCombatStats();

            if (attackCooldown == true) {

                AttackVector();
            }

           //StartBattle();
           //enemyFight();

            modelTransform = &placeToWorld;
            modelProject = &fightProject;
            uiFunc = &starIntersectUI;

            drawStarField();
            //drawStars();
            //drawGalaxyFog(7);


            modelTransform = &placeConstToWorld;

            srand(currentTime);

            DrawHpEnemyBar();
            modelTransform = &placeConstToWorld;//Враг

            if (isShakingHero) {

                float beamTime = 4. * (currentTime - shakeStartTimeHero) / shakeDurationHero;
                if (beamTime > 1.) beamTime = 1;
                for (int i = 0;i < starSet[currentEnemyID]->starsCords.size();i++)
                {
                    auto p1 = starSet[currentEnemyID]->starsCords[i];
                    point3d p2 = starSet[player_sign]->starsCords[i % starSet[player_sign]->starsCords.size()];

                   placeConstToWorld(p1, *starSet[currentEnemyID]);
                   placeHeroToWorld(p2, *starSet[currentEnemyID]);

                    p2.x = lerp(p1.x, p2.x, beamTime);
                    p2.y = lerp(p1.y, p2.y, beamTime);
                    p2.z = lerp(p1.z, p2.z, beamTime);


                    drawLine(p1, p2);
                }
            }
            

            //ProjectileUpdate(deltaTime);
            //FireProjectile();
            //WeaponRender();

            lastFrameTime = currentTime;

            if (isDamageTaken)
            {
                isDamageTaken = false;
                isShaking = true;
                shakeStartTime = currentTime;
            }

            if (currentTime > shakeStartTime + shakeDuration)
            {
                isShaking = false;
            }

            if (currentTime > attack_cooldown + 5000)
            {
                
                attackCooldown = true;
            }
            
            if (!GetAsyncKeyState(VK_LBUTTON))
            {
                if (attack_collision == true and attackCooldown == true)
                {
                    attack_cooldown = currentTime;
                    attackCooldown = false;
                    check_attack = false;
                    attackStartTime = currentTime;
                    
                }
                Constellation& h = *starSet[currentEnemyID];
                h.Transform = CreateEnemyToWorldMatrix(h);
                Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
                drawСonstellation(*starSet[currentEnemyID],false, 1000.f, 100.f);

                //linksDivider = 15;
                modelTransform = &placeHeroToWorld;
                uiFunc = &heroUI;
                Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

                //Constellation& c = *starSet[player_sign];
                //c.Transform = CreateHeroToWorldMatrix(c);
                //drawСonstellation(*starSet[player_sign]);//Игрок

                if (attack_collision == true)
                {
                    check_attack = true;
                    attack_collision = false;
                    attack_speed = false;
                }
            }
            else
            {
                uiFunc = NULL;
                Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
                Constellation& c = *starSet[currentEnemyID];

                c.Transform = CreateEnemyToWorldMatrix(c);
                drawСonstellation(*starSet[currentEnemyID],false, 1000.f, 100.f);
            }

            if (GetAsyncKeyState('P')) {
                gameState = gameState_::WinFight;
            }

            if (currentTime > attack_time + weapon[(int)current_weapon].attackSpeed and attack_start == true)
            {
                isDamageTaken = true;
                attack_start = false;
            }

            modelTransform = &placeConstToWorld;
            //DrawStarsHP(window.context);

            //linksDivider = 15;
            modelTransform = &placeHeroToWorld;
            uiFunc = &heroUI;

            modelTransform = NULL;
            uiFunc = NULL;

            if (isDamageHero)
            {
                isDamageHero = false;
                isShakingHero = true;
                shakeStartTimeHero = currentTime;
            }

            if (currentTime > shakeStartTimeHero + shakeDurationHero)
            {
                isShakingHero = false;
            }
            Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

            Constellation& c = *starSet[player_sign];
            c.Transform = CreateHeroToWorldMatrix(c);
            drawСonstellation(*starSet[player_sign]);

            std::string curentSignstring = zodiacSignToString(currentEnemyID);
            drawString(curentSignstring.c_str(), window.width / 1.1, window.height / 10., 1, true);

            curentSignstring = zodiacSignToString(player_sign);
            drawString(curentSignstring.c_str(), window.width / 2, window.height - window.height / 7., 1, true);

            curentSignstring = "current weapon: " + weapon[(int)current_weapon].name;
            drawString(curentSignstring.c_str(), window.width / 2, window.height - window.height / 10., 1, true);

            drawCurrentElement();

           drawString("Weapon selection:\nButton 1 - Sword \nButton 2 - Shield \nButton 3 - Bow ", (1700. / 2560) * window.width, (1100. / 1440) * window.height, .7f, false);
           drawString("Rewind time:\nbutton - R", (500. / 2560) * window.width, (1250. / 1440) * window.height, .7f, false);
           drawString("TUTORIAL:\nTo hit an enemy with a sword,\npress LMB and draw a line along the enemy star\nTo hit with a shield,\npress LMB and draw a line that will draw a circle that attacks stars\nTo hit with a bow,\npress LMB on the star and draw a vector in any direction from the star.", (60. / 2560) * window.width, (60. / 1440) * window.height, .7f, false);

            float cdTimeOut = 1. - (currentTime - attack_cooldown) / 5000.;
            cdTimeOut *= 10;
            std::string cdTimeOutText = std::to_string((int)cdTimeOut);
            if (cdTimeOut > 0)
            {
               drawString("recharge", window.width * .9, window.height * .85, 1., false);
               drawString(cdTimeOutText.c_str(), window.width * .9, window.height * .9, 3., false);
            }

            UpdateGame();

            break;
        }
        case gameState_::EndFight:
        {
            endFight();

            break;
        }
        case gameState_::WinFight:
        {
            //winFight();

            if (AriesNebula.defeatTime >= 0)
            {
                if (ConstBuf::factors.AriesNebulaLerpFactor < 1)
                {
                    ConstBuf::factors.AriesNebulaLerpFactor = min((ConstBuf::frame.time.x - AriesNebula.defeatTime) / 150.0f, 1);
                    ConstBuf::UpdateFactors();
                    ConstBuf::ConstToVertex(6);
                    ConstBuf::ConstToPixel(6);
                }
            }
            else
            {
                AriesNebula.defeatTime = ConstBuf::frame.time.x;
            }

            if (t) {
                t = false;
                Camera::state.camDist = 100;
            }
            Camera::state.mouse = true;
            Depth::Depth(Depth::depthmode::off);

            modelTransform = &placeToWorld;
            modelProject = &fightProject;
            uiFunc = &starIntersectUI;

            drawStarField();
            //drawStars();
            drawGalaxyFog(7);


            modelTransform = &placeConstToWorld;

            srand(currentTime);

            //DrawHpEnemyBar();
            modelTransform = &placeConstToWorld;//Враг

            lastFrameTime = currentTime;

            if (!GetAsyncKeyState(VK_LBUTTON))
            {
                if (attack_collision == true and attackCooldown == true)
                {
                    attack_cooldown = currentTime;
                    attackCooldown = false;
                    check_attack = false;
                    attackStartTime = currentTime;

                }
                Constellation& h = *starSet[currentEnemyID];
                h.Transform = CreateEnemyToWorldMatrix(h);
                Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
                drawСonstellation(*starSet[currentEnemyID]);

                //linksDivider = 15;
                modelTransform = &placeHeroToWorld;
                uiFunc = &heroUI;
                Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

                //Constellation& c = *starSet[player_sign];
                //c.Transform = CreateHeroToWorldMatrix(c);
                //drawСonstellation(*starSet[player_sign]);//Игрок

                if (attack_collision == true)
                {
                    check_attack = true;
                    attack_collision = false;
                    attack_speed = false;
                }
            }
            else
            {
                uiFunc = NULL;
                Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
                Constellation& c = *starSet[currentEnemyID];

                c.Transform = CreateEnemyToWorldMatrix(c);
                drawСonstellation(*starSet[currentEnemyID]);
            }

            modelTransform = &placeConstToWorld;
            //DrawStarsHP(window.context);

            //linksDivider = 15;
            modelTransform = &placeHeroToWorld;
            uiFunc = &heroUI;

            modelTransform = NULL;
            uiFunc = NULL;

            if (isDamageHero)
            {
                isDamageHero = false;
                isShakingHero = true;
                shakeStartTimeHero = currentTime;
            }

            if (currentTime > shakeStartTimeHero + shakeDurationHero)
            {
                isShakingHero = false;
            }
            Blend::Blending(Blend::blendmode::on, Blend::blendop::add);

            Constellation& c = *starSet[player_sign];
            c.Transform = CreateHeroToWorldMatrix(c);
            drawСonstellation(*starSet[player_sign]);

            std::string curentSignstring = zodiacSignToString(currentEnemyID);
            drawString(curentSignstring.c_str(), window.width / 1.1, window.height / 10., 1, true);

            curentSignstring = zodiacSignToString(player_sign);
            drawString(curentSignstring.c_str(), window.width / 2, window.height - window.height / 7., 1, true);


            //drawCurrentElement();

            //UpdateGame();

            break;
        }
        case gameState_::Exploring:
        {

            break;
        }

        }


        Depth::Depth(Depth::depthmode::off);
        if (gameState != gameState_::selectEnemy && gameState != gameState_::Fight && gameState != gameState_::WinFight)
        {
            drawCursor(6);
            CreateCursorParticles();
        }
        else
        {
            drawCursor(11);
        }

        DrawUiParticles(deltaTime);

    }
}

