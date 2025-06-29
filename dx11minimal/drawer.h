
namespace drawer
{
    void drawLine(point3d& p1, point3d& p2, int count)
    {
        if (count < 1) return;

        for (int i = 0;i < count;i++)
        {
            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            float dz = p2.z - p1.z;
            float length = sqrt(dx * dx + dy * dy + dz * dz);

            point3d point;
            float t = static_cast<float>(i) / count;
            point.x = p1.x + dx * t;
            point.y = p1.y + dy * t;
            point.z = p1.z + dz * t;

            if (modelProject)
            {
                //modelProject(point);
            }

            float sz = 1 + .5 * sinf(i + currentTime * .01);
            point.draw(point, sz);
            // Рисование Линий.
        }
    }

    float linksDivider = 25;

    void drawLinks(Constellation& Constellation, bool colorOverride = false)
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
                float dx = point2.x - point1.x;
                float dy = point2.y - point1.y;
                float dz = point2.z - point1.z;
                float length = sqrt(dx * dx + dy * dy + dz * dz);
                int x = static_cast<int>(length) / linksDivider;
                drawLine(point1, point2, x);// Рисование звёздных линий созвездия.
            }
        }
    }

    void drawStarPulse(Constellation& Constellation, bool colorOverride = false)
    {
        std::vector <point3d>& starArray = Constellation.starsCords;
        std::vector <float>& starHealth = Constellation.starsHealth;

        int starsCount = starArray.size();
        if (!colorOverride) {
            brush = CreateSolidBrush(RGB(0, 191, 255));
            brush2 = CreateSolidBrush(RGB(255, 0, 0));
            SelectObject(window.context, brush);
        }

        for (int i = 0; i < starsCount; i++)
        {
            point3d point;
            point = starArray[i];
            point3d worldPoint = TransformPoint(point, Constellation.Transform);
            point3d screenPoint = TransformPoint(worldPoint, Constellation.Project);
            if (T) {
                point3d screenPoint = TransformPoint(worldPoint, Constellation.Project);
            }
            // Пульсирование Звёзд при наведение мыши.
            finalStarRad = 1;
            //point = TransformPoint(point, ConstBuf::camera.view[0]);
            if (uiFunc)
            {
                uiFunc(screenPoint, Constellation, i);
            }

            
            if (finalStarRad > 0)
            {
                point.draw(worldPoint, finalStarRad);
            }
        }

        if (!colorOverride)
        {
            DeleteObject(brush);
            DeleteObject(brush2);
        }
    }

    void drawСonstellation(Constellation& Constellation, bool colorOverride = false)
    {

        drawLinks(Constellation, colorOverride);
        drawStarPulse(Constellation, colorOverride);
    }

    void(drawStarField())
    {
        Shaders::vShader(2);
        Shaders::pShader(2);
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

    HBRUSH colorBrush[numColors];

    void drawColorCircle()
    {
        circleRadius = 175;
        float centerX = window.width / 4;
        float centerY = window.height / 4;

        int numColors = sizeof(colors) / sizeof(colors[0]);
        float angleStep = 2 * PI / (float)numColors;

        for (int i = 0; i < numColors; ++i) {

            float angle1 = -i * angleStep;
            float angle2 = -(i + 1) * angleStep;

            POINT p1, p2;
            p1.x = centerX + circleRadius * cos(angle1);
            p1.y = centerY + circleRadius * sin(angle1);

            p2.x = centerX + circleRadius * cos(angle2);
            p2.y = centerY + circleRadius * sin(angle2);

            HBRUSH brush = CreateSolidBrush(colors[i]);
            SelectObject(window.context, brush);

            Pie(window.context,
                centerX - circleRadius,
                centerY - circleRadius,
                centerX + circleRadius,
                centerY + circleRadius,
                p1.x, p1.y,
                p2.x, p2.y);

            DeleteObject(brush);// Данные для рисование цвет. круга.
        }
    }

    void drawGameCursor()
    {
        HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 191, 255));
        HBRUSH brush = CreateSolidBrush(RGB(0, 191, 255));
        SelectObject(window.context, pen);
        SelectObject(window.context, brush);

        for (int i = 0; i < numColors;i++)
        {
            colorBrush[i] = CreateSolidBrush(colors[i]);
        }

        if (currentColorIndex == -1)
        {
            SelectObject(window.context, brush);
        }
        else
        {
            SelectObject(window.context, colorBrush[currentColorIndex]);
        }

        Ellipse(window.context,
            mouse.pos.x - starSize,
            mouse.pos.y - starSize,
            mouse.pos.x + starSize,
            mouse.pos.y + starSize
        );

        for (int i = 0; i < numColors;i++)
        {
            DeleteObject(colorBrush[i]);
        }

        DeleteObject(pen);
        DeleteObject(brush);

    }



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
        linksDivider = 15;
        if (gameState == gameState_::confirmSign) n = player_sign;
        Constellation& c = *starSet[n];
        float t = currentTime * 0.001;
        c.Transform = XMMatrixScaling(90, 90, 90) * XMMatrixRotationY(t) * XMMatrixTranslation(0, 0, 1300);
        drawСonstellation(*starSet[player_sign]);
    }

    void drawShieldCircle()
    {
        float dx = mouse.pos.x - mouse.oldPos.x;
        float dy = mouse.pos.y - mouse.oldPos.y;
        float shieldRadius = sqrt(dx * dx + dy * dy);

        // Центр = oldmouse (круг растёт из этой точки)
        float centerX = mouse.oldPos.x;
        float centerY = mouse.oldPos.y;
        modelProject = &NullProject;
        if (GetAsyncKeyState(VK_LBUTTON)) {
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

                drawLine(shield1, shield2, 5);
            }
        }
    }

    struct WeaponFlight {
        point3d startPos;
        point3d endPos;
        point3d currentPos;
        float progress;
        float speed;
        bool isFlying;
    };

    WeaponFlight constellationFlight;

    point3d startPoint;

    float lerp(float start, float end, float t) {
        return start + (end - start) * t;
    }
    void VectorWeapons(point3d& p, Constellation& Constellations)
    {

        point3d enemyPosition = { 0,0,0 };


        if (weapon[(int)current_weapon].constellation)
        {
            p = weapon[(int)current_weapon].constellation->getPosition();

            startPoint = { 0, 0, 0 };
            placeHeroToWorld(startPoint, *starSet[player_sign]);

            int сount = 100;
            placeConstToWorld(enemyPosition, *starSet[currentEnemyID]);
            modelProject = &fightProject;

            drawLine(startPoint, enemyPosition, сount);

        }
    }

    void InitConstellationAttack() {

        point3d mouseAngleBackup = mouse.Angle;
        mouse.Angle.x = 0;
        mouse.Angle.y = 0;

        point3d heroPos = { 0, 0, 0 };

        placeHeroToWorld(heroPos, *starSet[player_sign]);

        point3d enemyPos = { 0, 0, 0 };
        placeConstToWorld(enemyPos, *starSet[currentEnemyID]);

        constellationFlight.startPos = heroPos;
        constellationFlight.endPos = enemyPos;
        constellationFlight.currentPos = heroPos;
        constellationFlight.progress = 0.0f;
        constellationFlight.speed = 0.05f;
        constellationFlight.isFlying = true;

        mouse.Angle = mouseAngleBackup;
    }

    void ReturnHeroConstellation() {
        point3d homePos = { 0, 0, 0 };
        placeHeroToWorld(homePos, *starSet[player_sign]);

        for (auto& star : starSet[player_sign]->starsCords) {
            star = homePos;
        }
    }

    void UpdateConstellationAttack() {
        if (!constellationFlight.isFlying) return;

        constellationFlight.progress += constellationFlight.speed;

        float steps = 1. / constellationFlight.speed;
        float dx = .008 * (constellationFlight.endPos.x - constellationFlight.startPos.x) / steps;
        float dy = .008 * (constellationFlight.endPos.y - constellationFlight.startPos.y) / steps;
        float dz = .008 * (constellationFlight.endPos.z - constellationFlight.startPos.z) / steps;

        // Параболическая траектория
        /*float t = constellationFlight.progress;
        float height = 300.0f * sin(t * PI); // Высота дуги

        constellationFlight.currentPos.x = lerp(constellationFlight.startPos.x, constellationFlight.endPos.x, t);
        constellationFlight.currentPos.y = lerp(constellationFlight.startPos.y, constellationFlight.endPos.y, t);// +height;
        constellationFlight.currentPos.z = lerp(constellationFlight.startPos.z, constellationFlight.endPos.z, t);
        */

        // Перемещаем всё созвездие героя
        for (auto& star : starSet[player_sign]->starsCords) {
            star.x += dx;
            star.y += dy;
            star.z += dz;
        }

        if (constellationFlight.progress >= 1.0f) {
            constellationFlight.isFlying = false;
            //enemyAttack(*starSet[currentEnemyID]);

            // Возвращаем созвездие на место
            //ReturnHeroConstellation();
            for (auto& star : starSet[player_sign]->starsCords) {
                star.x -= dx * steps;
                star.y -= dy * steps;
                star.z -= dz * steps;
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

        linksDivider = 15;
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
        drawLine(topLeft, topRight, 50);    // Верх
        drawLine(bottomLeft, bottomRight, 50); // Низ
        drawLine(topLeft, bottomLeft, 5);  // Лево
        drawLine(bottomRight, topRight, 5); // Право

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

            int lineSegments = ((i < activeStars - 1) ? 10 : 5);
            drawLine(star1, star2, lineSegments);
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

        linksDivider = 15;
        modelTransform = &HeroUITransform;
        uiFunc = NULL;
        nearPlaneClip = -2000;
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
        drawer::drawLine(topLeft, topRight, 50);    // Верх
        drawer::drawLine(bottomLeft, bottomRight, 50); // Низ
        drawer::drawLine(topLeft, bottomLeft, 5);  // Лево
        drawer::drawLine(bottomRight, topRight, 5); // Право

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

            int lineSegments = (i < activeStars - 1) ? 10 : 5;
            drawer::drawLine(star1, star2, lineSegments);
        }

        point3d p = { 0,2,0 };
        modelTransform(p, player_const);
        modelProject(p);
        drawString(progressText.c_str(), p.x, p.y, 1, true);
    }

    bool isBattleActive = false;
    DWORD battleStartTime;


    void StartBattle() {
        if (!isBattleActive) {
            battleStartTime = currentTime;
            attackTime = battleStartTime;
            isBattleActive = true;
            //TextOutA(window.context, 400, 400, "Бой начался", 10);
            drawString("Start Fight", 400, 400, 1, true);
        }
    }

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
        nearPlaneClip = 0;

        SelectObject(window.context, mainBrush);
        SelectObject(window.context, mainPen);
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
            drawPlayerСonstellationToMenu();
            menuMonthprocessing();
            menuDayprocessing();
            menuConfirmationButton();
            break;

        case gameState_::selectEnemy:
        {
            
            uiFunc = &constSelectUI;
            linksDivider = 50;
            drawStarField();
            Shaders::vShader(1);
            Shaders::pShader(1);
            //modelProject = &fightProject;
            //modelTransform = &placeConstToWorld;
            

            for (int i = 0;i < 12;i++)
            {
                Constellation& c = *starSet[i];
                c.Transform = CreateConstToWorldMatrix(c);
                T = true;
                c.Project = CreateWorldToScreenMatrix(c);
               drawСonstellation(*starSet[i]);
            }

            std::string curentSignstring = zodiacSignToString(player_sign);
            TextOutA(window.context, window.width * 5 / 6, window.height - window.height / 20., curentSignstring.c_str(), curentSignstring.size());
            //drawString(curentSignstring, window.width * 5 / 6, window.height - window.height / 20., 1, false);

            drawString("Find Constallations and click on him", window.width / 2, (200. / 1440) * window.height, 1, true);
            drawString("Features:\nMouse wheel to zoom in and out", (1700. / 2560) * window.width, (1200. / 1440) * window.height, .7f, false);
            // drawColorCircle();
            isBattleActive = false;

            break;
        }
        case gameState_::DialogStruct:
            initContentData();
            renderContent();
            handleInput();

            break;

        case gameState_::Fight:
        {
            SelectWeapon();
            SelectElement();


            DrawCombatStats();

            if (attackCooldown == true) {

                AttackVector();

                if (current_weapon == weapon_name::Shield)
                {
                    drawShieldCircle();
                }
            }

            StartBattle();
            enemyFight();

            modelTransform = &placeToWorld;
            modelProject = &fightProject;
            uiFunc = &starIntersectUI;
            linksDivider = 50;

            drawStarField();


            modelTransform = &placeConstToWorld;
            nearPlaneClip = 0;

            srand(currentTime);

            DrawHpEnemyBar();
            modelTransform = &placeConstToWorld;//Враг

            nearPlaneClip = 0;

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


                    drawLine(p1, p2, 50);
                }
            }

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

            if (constellationFlight.isFlying)
            {
                UpdateConstellationAttack();
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
                    InitConstellationAttack();
                }

                drawСonstellation(*starSet[currentEnemyID]);

                linksDivider = 15;
                modelTransform = &placeHeroToWorld;
                uiFunc = &heroUI;
                nearPlaneClip = -2000;
                SelectObject(window.context, heroBrush);
                SelectObject(window.context, heroPen);

                drawСonstellation(*starSet[player_sign], true);//Игрок

                SelectObject(window.context, mainBrush);
                SelectObject(window.context, mainPen);

                if (attack_collision == true)
                {
                    check_attack = true;
                    attack_collision = false;
                    attack_speed = false;
                }
            }
            else
            {
                drawСonstellation(*starSet[currentEnemyID]);
            }

            if (currentTime > attack_time + weapon[(int)current_weapon].attackSpeed and attack_start == true)
            {
                isDamageTaken = true;
                attack_start = false;
            }

            SelectObject(window.context, mainBrush);
            SelectObject(window.context, mainPen);

            modelTransform = &placeConstToWorld;
            DrawStarsHP(window.context);

            linksDivider = 5;
            modelTransform = &placeHeroToWorld;
            uiFunc = &heroUI;
            nearPlaneClip = -2000;

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

            SelectObject(window.context, heroBrush);
            SelectObject(window.context, heroPen);

            drawСonstellation(*starSet[player_sign], true);

            SelectObject(window.context, mainBrush);
            SelectObject(window.context, mainPen);


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
        }
        drawGameCursor();

    }
}

