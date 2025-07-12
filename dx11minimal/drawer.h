bool t = true;
namespace drawer
{
    void drawLine(point3d& p1, point3d& p2)
    {
        point3d vector = p2 - p1;
        point3d mid = p1.lerp(p2, 0.5f);

        float sz = 1;

        ConstBuf::ConstToVertex(4);
        ConstBuf::ConstToPixel(4);


        ConstBuf::global[0] = XMFLOAT4(p1.x, p1.y, p1.z, 1.0f);
        ConstBuf::global[1] = XMFLOAT4(p2.x, p2.y, p2.z, 1.0f);

        ConstBuf::Update(5, ConstBuf::global);
        ConstBuf::ConstToVertex(5);

        Draw::elipse(1);
    }

    void drawLinks(Constellation& Constellation)
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
                drawLine(point1, point2);
            }
        }
    }

    void drawStarPulse(Constellation& Constellation, bool colorOverride = false)
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
            finalStarRad = 1;
            if (uiFunc)
            {
                uiFunc(screenPoint, Constellation, i);
            }

            
            if (finalStarRad > 0)
            {
                point.draw(worldPoint, finalStarRad);
            }
        }
    }

    void drawСonstellation(Constellation& Constellation, bool colorOverride = false)
    {
        Shaders::vShader(1);
        Shaders::pShader(1);

        drawStarPulse(Constellation, colorOverride);

        Shaders::vShader(4);
        Shaders::pShader(4);

        drawLinks(Constellation);
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



    struct Projectile {
        point3d currentPos;
        point3d targetPos;
        point3d position;
        point3d direction;
        float lifetime;
        float speed;
        bool isActive;
        float size;
        COLORREF color;
        weapon_name weaponType;
        element_name elementType;
        float damage;

        void Init(const point3d& start, const point3d& target) {
            position = start;
            direction = (target - start).normalized();
            speed = 50.0f;
            isActive = true;
            lifetime = 5.0f; // 5 секунд жизни
        
            // Настройки в зависимости от оружия
            switch (weaponType) {
            case weapon_name::Sword:
                speed = 10.f;
                size = 15.0f;
                color = RGB(255, 255, 0); // Желтый для меча
                break;
            case weapon_name::Bow:
                speed = 500.f;
                size = 8.0f;
                color = RGB(200, 200, 200); // Серый для лука
                break;
            case weapon_name::Staff:
                speed = 250.f;
                size = 20.0f;
                // Цвет зависит от элемента
                color = elements[(int)elementType].color;
                break;
            case weapon_name::Shield:
                speed = 20.f;
                size = 25.0f;
                color = RGB(100, 100, 255); // Синий для щита
                break;
            }

            isActive = true;
        }

        float CalculateProjectileDamage() {
            float baseDamage = weapon[(int)weaponType].damage;
            float elementMod = elements[(int)elementType].damageMod;
            float weaponElementMod = 1.0f;

            switch (weaponType) {
            case weapon_name::Sword: weaponElementMod = elements[(int)elementType].swordMod; break;
            case weapon_name::Shield: weaponElementMod = elements[(int)elementType].shieldMod; break;
            case weapon_name::Bow: weaponElementMod = elements[(int)elementType].bowMod; break;
            case weapon_name::Staff: weaponElementMod = elements[(int)elementType].staffMod; break;
            }

            return baseDamage * elementMod * weaponElementMod;
        }

        void Update(float deltaTime) {
            if (!isActive) return;

            // Движение вперед
            position += direction * speed * deltaTime;

            // Уменьшение времени жизни
            lifetime -= deltaTime;
            if (lifetime <= 0) isActive = false;
        }

        void OnHitTarget() {
            // Здесь можно добавить эффекты попадания
            // и нанесение урона врагу
        }
    };


    std::vector<Projectile> projectiles; // Вектор для хранения снарядов


    point3d ScreenToWorld(point3d screenPos) {
        // 1. Получаем матрицы вида и проекции
        XMMATRIX view = ConstBuf::camera.view[0];
        XMMATRIX proj = ConstBuf::camera.proj[0];

        // 2. Инвертируем матрицу вида-проекции
        XMMATRIX invViewProj = XMMatrixInverse(nullptr, view * proj);

        // 3. Преобразуем экранные координаты в NDC
        float x = (2.0f * screenPos.x) / window.width - 1.0f;
        float y = 1.0f - (2.0f * screenPos.y) / window.height;

        // 4. Создаем луч из камеры
        XMVECTOR rayOrigin = XMVector3Unproject(
            XMVectorSet(screenPos.x, screenPos.y, 0.0f, 0.0f),
            0, 0, window.width, window.height, 0.0f, 1.0f,
            proj, view, XMMatrixIdentity());

        XMVECTOR rayEnd = XMVector3Unproject(
            XMVectorSet(screenPos.x, screenPos.y, 1.0f, 0.0f),
            0, 0, window.width, window.height, 0.0f, 1.0f,
            proj, view, XMMatrixIdentity());

        // 5. Нормализуем направление
        XMVECTOR rayDir = XMVector3Normalize(rayEnd - rayOrigin);

        // 6. Возвращаем точку на фиксированном расстоянии
        XMVECTOR worldPos = rayOrigin + rayDir * 1000.0f;

        return point3d{
            XMVectorGetX(worldPos),
            XMVectorGetY(worldPos),
            XMVectorGetZ(worldPos)
        };
    }


    // Модифицированная функция атаки
    void FireProjectile() {
        if (GetAsyncKeyState(VK_LBUTTON) && !lmb) {
            lmb = true;

            // 1. Получаем позицию игрока (например, центр его созвездия)
            Constellation* player = starSet[player_sign];
            point3d startPos = player->position;

            // 2. Получаем мировую позицию курсора
            point3d targetPos = ScreenToWorld(mouse.pos);

            // 3. Корректируем направление от игрока к курсору
            XMVECTOR dir = XMVector3Normalize(XMVectorSet(
                targetPos.x - startPos.x,
                targetPos.y - startPos.y,
                targetPos.z - startPos.z, 0));

            // 4. Создаем снаряд
            Projectile proj;
            proj.Init(startPos, targetPos);
            projectiles.push_back(proj);
        }
        else if (!GetAsyncKeyState(VK_LBUTTON)) {
            lmb = false;
        }
    }

    

    void ProjectileUpdate(float deltaTime) {
        // Обновление снарядов
        for (auto& proj : projectiles) {
            proj.Update(deltaTime);

            // Проверка столкновений
            //CheckCollision(proj);
        }

        // Удаление неактивных снарядов
        projectiles.erase(
            std::remove_if(projectiles.begin(), projectiles.end(),
                [](const Projectile& p) { return !p.isActive; }),
            projectiles.end());
    }

    void CreateProjectile() {
        // 1. Позиция игрока (например, центр его созвездия)
        Constellation* player = starSet[player_sign];
        point3d startPos = player->position;

        // 2. Получаем направление от камеры к курсору
        point3d target = ScreenToWorld(mouse.pos);
        XMVECTOR eye = Camera::state.Eye;

        XMVECTOR dir = XMVector3Normalize(XMVectorSet(
            target.x - XMVectorGetX(eye),
            target.y - XMVectorGetY(eye),
            target.z - XMVectorGetZ(eye),
            0));

        // 3. Создаем снаряд
        Projectile proj;
        proj.position = startPos;
        proj.direction = point3d{
            XMVectorGetX(dir),
            XMVectorGetY(dir),
            XMVectorGetZ(dir)
        };
        proj.speed = 50.0f; // Скорость полета
        proj.isActive = true;

        projectiles.push_back(proj);
    }
    void WeaponRender() {
        for (const auto& proj : projectiles) {
            if (proj.isActive) {
                point3d drawPos = proj.currentPos;
                modelProject(drawPos);

                // Отрисовка в зависимости от типа оружия
                switch (proj.weaponType) {
                case weapon_name::Sword:
                    drawSwordLine(drawPos.x, drawPos.y);
                    break;
                case weapon_name::Shield:
                    drawShieldCircle(drawPos.x, drawPos.y, drawPos.z);
                    break;
                case weapon_name::Bow:
                    drawBowLine(drawPos.x, drawPos.y);
                    drawArrow(drawPos.x, drawPos.y);
                    break;
                case weapon_name::Staff:
                    // Для посоха используем цвет элемента
                    HBRUSH brush = CreateSolidBrush(proj.color);
                    float sz = proj.size;
                    Ellipse(window.context,
                        (int)(drawPos.x - sz),
                        (int)(drawPos.y - sz),
                        (int)(drawPos.x + sz),
                        (int)(drawPos.y + sz));
                    DeleteObject(brush);
                    break;
                }
            }
        }
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


    void drawWorld()
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
            //linksDivider = 15;
            
            drawStarField();
            Shaders::vShader(1);
            Shaders::pShader(1);
            //modelProject = &fightProject;
            //modelTransform = &placeConstToWorld;
            
            Constellation& c = *starSet[player_sign];
            c.Transform = CreateHeroToWorldMatrix(c);
            drawСonstellation(*starSet[player_sign]);//Игрок

            for (int i = 0;i < 12;i++)
            {
               Constellation& c = *starSet[i];
               c.Transform = CreateConstToWorldMatrix(c);
               drawСonstellation(*starSet[i]);
            }

            std::string curentSignstring = zodiacSignToString(player_sign);
            TextOutA(window.context, window.width * 5 / 6, window.height - window.height / 20., curentSignstring.c_str(), curentSignstring.size());
            //drawString(curentSignstring, window.width * 5 / 6, window.height - window.height / 20., 1, false);

            drawString("Find Constallations and click on him", window.width / 2, (200. / 1440) * window.height, 1, true);
            drawString("Features:\nMouse wheel to zoom in and out", (1700. / 2560) * window.width, (1200. / 1440) * window.height, .7f, false);
            // drawColorCircle();
            isBattleActive = false;


           // drawString("X", 0, 0, 1, false);
           // drawString(std::to_string(mouse.pos.y).c_str(), 0, 0, 1, false);

            break;
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
            //linksDivider = 50;

            drawStarField();
            drawStars();


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
            point3d start = starSet[player_sign]->position;

            point3d camPos = point3d(XMVectorGetX(Camera::state.Eye), XMVectorGetY(Camera::state.Eye), XMVectorGetZ(Camera::state.Eye));
            point3d mouseRay = GetMouseRay(mouse.pos);
            point3d mousePos = camPos + mouseRay * 1000;
            point3d end = start + (mousePos - start);

            string info = std::to_string(mouseRay.x) + " " + std::to_string(mouseRay.y) + " " + std::to_string(mouseRay.z);
            drawString(info.c_str(), 1500, 50, 1.0f, true);

            Shaders::vShader(4);
            Shaders::pShader(4);
            drawLine(start, end);

            float deltaTime = (currentTime - lastFrameTime) / 1000.0f;
            ProjectileUpdate(deltaTime);
            FireProjectile();
            WeaponRender();

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

            if (currentTime > attack_time + weapon[(int)current_weapon].attackSpeed and attack_start == true)
            {
                isDamageTaken = true;
                attack_start = false;
            }

            modelTransform = &placeConstToWorld;
            DrawStarsHP(window.context);

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
           drawString("Rewind time:\nbutton - E", (500. / 2560) * window.width, (1200. / 1440) * window.height, .7f, false);
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
            winFight();
            break;
        }
        case gameState_::Exploring:
        {

            break;
        }

        }

    }
}

