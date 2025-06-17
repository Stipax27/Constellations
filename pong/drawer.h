typedef void(*PVFN)(point3d& point);

namespace drawer
{ 
    float linksDivider = 25;

    void drawLinks(Entity& entity, bool colorOverride = false) {
        if (!entity.constellation || !entity.healthSystem) return;

        std::vector<point3d>& starArray = entity.constellation->starsCords;
        std::vector<std::vector<float>>& starEdges = entity.constellation->constellationEdges;
        std::vector<int>& starHealth = entity.healthSystem->starsHealth;

        if (starArray.size() < starEdges.size() || starHealth.size() < starArray.size()) return;

        int starsEdgesCount = static_cast<int>(starEdges.size());
        for (int i = 0; i < starsEdgesCount; i++) {
           
            if (starEdges[i].size() < 2 ||
                starEdges[i][0] >= starArray.size() ||
                starEdges[i][1] >= starArray.size()) continue;

            point3d point1 = starArray[starEdges[i][0]];
            point3d point2 = starArray[starEdges[i][1]];

            float a = currentTime * 0.01f;
            modelTransform(point1, *entity.constellation);
            modelTransform(point2, *entity.constellation);

            if (starHealth[starEdges[i][0]] > 0 && starHealth[starEdges[i][1]] > 0) {
                float dx = point2.x - point1.x;
                float dy = point2.y - point1.y;
                float dz = point2.z - point1.z;
                float length = sqrt(dx * dx + dy * dy + dz * dz);
                int x = static_cast<int>(length) / linksDivider;
                drawLine(point1, point2, x);
            }
        }
    }

    void drawStarPulse(Entity& entity, bool colorOverride = false) {
        if (!entity.constellation || !entity.healthSystem) return;

        std::vector<point3d>& starArray = entity.constellation->starsCords;
        std::vector<int>& starHealth = entity.healthSystem->starsHealth;

        
        if (starArray.size() != starHealth.size()) return;

        int starsCount = static_cast<int>(starArray.size());
        if (!colorOverride) {
            brush = CreateSolidBrush(RGB(0, 191, 255));
            brush2 = CreateSolidBrush(RGB(255, 0, 0));
            SelectObject(window.context, brush);
        }

        for (int i = 0; i < starsCount; i++) {
            point3d point = starArray[i];

            float a = currentTime * 0.01f;
            modelTransform(point, *entity.constellation);
            modelProject(point);

           
            finalStarRad = 1;
            if (uiFunc) {
               
                uiFunc(point, *entity.constellation, &entity, i);
            }

            if (finalStarRad > 0) {
                point.draw(point, finalStarRad);
            }
        }

        if (!colorOverride) {
            DeleteObject(brush);
            DeleteObject(brush2);
        }
    }

    void draw�onstellation(Entity& entities, bool colorOverride = false)
    {
        drawLinks(entities, colorOverride);
        drawStarPulse(entities, colorOverride);
    }

    void drawStarField()
    {
        SelectObject(window.context, mainBrush);
        SelectObject(window.context, mainPen);

        srand(10);
        for (int i = 0; i < 2000; i++)
        {
            point3d point;
            genRandSphere(point);
  
            modelTransform(point,Aries);
            point.rotateZ(point, starfield_angles.z);
            modelProject(point);

            point.draw(point,2);
            // ����� �� ���� �� ���-��. � ���� ���������.
        }
    }

    void drawMilkyWay()
    {
        HBRUSH brashStar;
        HPEN penStar;
        srand(time(0));
        brashStar = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
        penStar = CreatePen(PS_SOLID, 2, RGB(rand() % 256, rand() % 256, rand() % 256));
        SelectObject(window.context, brashStar);
        SelectObject(window.context, penStar);

        srand(10); 
        for (int i = 0; i < 1000; i++)
        {
            point3d point;
            genWaySphere(point);
            modelTransform(point, Aries);
            modelProject(point);

            int starSize = rand() % 3 + 1; 

            point.draw(point, starSize);
        }
    }

    const COLORREF colors[] =
    {
        RGB(255, 0, 0),    // �������
        RGB(255, 165, 0),  // ���������
        RGB(255, 255, 0),  // ������
        RGB(0, 255, 0),    // �������
        RGB(0, 255, 255),  // �������
        RGB(0, 0, 255),    // �����
        RGB(128, 0, 128)   // ����������
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

            DeleteObject(brush);// ������ ��� ��������� ����. �����.
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

    void drawBack()
    {
        RECT rect;
        GetClientRect(window.hWnd, &rect);
        auto blackBrush = CreateSolidBrush(RGB(0, 15, 30));
        FillRect(window.context, &rect, blackBrush);
        DeleteObject(blackBrush);
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
        
        //draw�onstellation(morphArray, Morp_indices, Morp_health); ���������
    }

    void drawPlayer�onstellationToMenu(Entity& entities)
    {
        //Entity* playerEntity = &entities[static_cast<int>(player_sign)];

        startTime = currentTime;
        int n = (currentTime / 1000) % starSet.size();
        modelTransform = &placeConstToStartMenu;
        modelProject = &menuProject;
        uiFunc = &menuUI;
        linksDivider = 15;
        if (gameState == gameState_::confirmSign) n = player_sign;
        //draw�onstellation(*starSet[n]);
        draw�onstellation(entities);
        //draw�onstellation(*starSet[player_sign]);
    }

    void drawShieldCircle() 
    {
        float dx = mouse.pos.x - mouse.oldPos.x;
        float dy = mouse.pos.y - mouse.oldPos.y;
        float shieldRadius = sqrt(dx * dx + dy * dy);

        // ����� = oldmouse (���� ����� �� ���� �����)
        float centerX = mouse.oldPos.x;
        float centerY = mouse.oldPos.y;
        modelProject = &NullProject;
        if (GetAsyncKeyState(VK_LBUTTON)) {
            for (int i = 0; i < 36; i++) {
                float angle = i * (2 * PI / 36);  // 36 ����� ��� �������� �����
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

            int �ount = 100;
            placeConstToWorld(enemyPosition, *starSet[currentEnemyID]);
            modelProject = &fightProject;

            drawLine(startPoint, enemyPosition, �ount);

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
        float dx = .008*(constellationFlight.endPos.x - constellationFlight.startPos.x) / steps;
        float dy = .008 * (constellationFlight.endPos.y - constellationFlight.startPos.y) / steps;
        float dz = .008 * (constellationFlight.endPos.z - constellationFlight.startPos.z) / steps;

        // �������������� ����������
        /*float t = constellationFlight.progress;
        float height = 300.0f * sin(t * PI); // ������ ����

        constellationFlight.currentPos.x = lerp(constellationFlight.startPos.x, constellationFlight.endPos.x, t);
        constellationFlight.currentPos.y = lerp(constellationFlight.startPos.y, constellationFlight.endPos.y, t);// +height;
        constellationFlight.currentPos.z = lerp(constellationFlight.startPos.z, constellationFlight.endPos.z, t);
        */

        // ���������� �� ��������� �����
        for (auto& star : starSet[player_sign]->starsCords) {
            star.x += dx;
            star.y += dy;
            star.z += dz;
        }

        if (constellationFlight.progress >= 1.0f) {
            constellationFlight.isFlying = false;
            //enemyAttack(*starSet[currentEnemyID]);

            // ���������� ��������� �� �����
            //ReturnHeroConstellation();
            for (auto& star : starSet[player_sign]->starsCords) {
                star.x -= dx * steps;
                star.y -= dy * steps;
                star.z -= dz * steps;
            }
        }
    }


    void UpdateEnemyOrbitPosition();
    void InitEnemyOrbit();
    // ���������� ���������� ��� �������� ��������� ����� �����
    struct EnemyOrbit {
        point3d centerPos;    // ����� ��������� �����
        float radius;         // ������ ������
        float angle;          // ������� ����
        float speed;          // �������� ��������
        bool isActive;

        // ��������� ������������ �������� ����
        std::vector<point3d> starOffsets;
    };

    EnemyOrbit enemyOrbit;

    // 2. ������� ������������� ������ � ����������� ��������� ���������
    void InitEnemyOrbit() {
        // �������� ����� ��������� �����
        enemyOrbit.centerPos = { 0, 0, 0 };
        placeConstToWorld(enemyOrbit.centerPos, *starSet[currentEnemyID]);

        // ��������� ������
        enemyOrbit.radius = 300.0f;
        enemyOrbit.angle = 0.0f;
        enemyOrbit.speed = 0.03f;
        enemyOrbit.isActive = true;

        // ��������� ������������ �������� ���� ������������ ������
        enemyOrbit.starOffsets.clear();
        for (const auto& star : starSet[currentEnemyID]->starsCords) {
            point3d offset = star - enemyOrbit.centerPos;
            enemyOrbit.starOffsets.push_back(offset);
        }

        UpdateEnemyOrbitPosition();
    }

    // 3. ����������� ������� �������� � ����������� ��������� ���������
    void UpdateEnemyOrbitPosition() {
        if (!enemyOrbit.isActive) return;

        // ��������� ����
        enemyOrbit.angle += enemyOrbit.speed;
        if (enemyOrbit.angle > 2 * PI) enemyOrbit.angle -= 2 * PI;

        // ��������� ����� ������� ������ ���������
        point3d newCenter;
        newCenter.x = enemyOrbit.centerPos.x + enemyOrbit.radius * cos(enemyOrbit.angle);
        newCenter.y = enemyOrbit.centerPos.y + 50.0f * sin(enemyOrbit.angle * 2.0f); // ��������� "��������"
        newCenter.z = enemyOrbit.centerPos.z + enemyOrbit.radius * sin(enemyOrbit.angle);

        // ��������� ������� ���� ����, �������� �� ������������� ��������
        for (size_t i = 0; i < starSet[currentEnemyID]->starsCords.size(); i++) {
            starSet[currentEnemyID]->starsCords[i] = newCenter + enemyOrbit.starOffsets[i];
        }
    }

    

    void DrawStarsHP(HDC hdc, Entity& entities) {

        Constellation* enemyConstellation = starSet[currentEnemyID];

        //Entity* enemyEntity = &entities[static_cast<int>(currentEnemyID)];

        for (size_t i = 0; i < enemyConstellation->starsCords.size(); ++i) {

            auto pos = enemyConstellation->starsCords[i];
            float hp = entities.healthSystem->starsHealth[i];

            modelTransform(pos, *enemyConstellation);
            modelProject(pos);

            if (hp > 0) {
                std::string hpText = "HP: " + std::to_string(hp);
                drawString(hpText.c_str(), pos.x, pos.y - 40, .5, true, false, 7);
            }
        }
    }
    void DrawHpEnemyBar(Entity& entities)
    {
        Constellation* enemyConstellation = starSet[currentEnemyID];

        //Entity* enemyEntity = &entities[static_cast<int>(currentEnemyID)];

        auto maxHP = entities.healthSystem->maxHP;
        auto currentHP = entities.healthSystem->starHP;

        float progress = (float)currentHP / (float)maxHP;
        auto progressText = "HP: " + std::to_string(currentHP) + "/" + std::to_string(maxHP);

        linksDivider = 15;
        modelTransform = &placeConstToWorld;
        uiFunc = starIntersectUI;
        modelProject = &fightProject;

        // ��������� ���������� ����
        const float barWidth = 300;
        const float barHeight = 60;
        const float barX = 0;
        const float barY = .3;
        const int starCount = 10;
        const int starSize = 5;

        point3d Bar = { barX, barY,0 };

        modelTransform(Bar, *starSet[currentEnemyID]);
        modelProject(Bar);

        // ������� ����� ��� �����
        point3d topLeft = { Bar.x - barWidth/2, Bar.y, 0 };
        point3d topRight = { Bar.x + barWidth / 2, Bar.y, 0 };
        point3d bottomLeft = { Bar.x - barWidth / 2, Bar.y + barHeight, 0 };
        point3d bottomRight = { Bar.x + barWidth /2, Bar.y + barHeight, 0 };

        modelProject = NULL;
        // ������ �����
        drawLine(topLeft, topRight, 50);    // ����
        drawLine(bottomLeft, bottomRight, 50); // ���
        drawLine(topLeft, bottomLeft, 5);  // ����
        drawLine(bottomRight, topRight, 5); // �����

        // ������ ���������� �� ����
        int activeStars = (int)(starCount * progress);
        float starSpacing = barWidth / (float)(starCount + 1);

        for (int i = 0; i < starCount; i++) {
            point3d star = {
                Bar.x + (i + 1) * starSpacing -barWidth / 2,
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

        // ��������� �����
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
        modelTransform(p, *enemyConstellation);
        modelProject(p);
        drawString(progressText.c_str(), p.x, p.y-50, 1, true);

    }

    void DrawHpHeroBar(Entity& entity)
    {
        Constellation* playerConstellation = starSet[player_sign];

        // �������������� starHP � ��������� ��������� �� starsHealth
        int totalStarsHealth = 0;
        for (int health : entity.healthSystem->starsHealth) {
            totalStarsHealth += health;
        }
        entity.healthSystem->starHP = totalStarsHealth;  

        auto maxHP = entity.healthSystem->maxHP;
        auto currentHP = entity.healthSystem->starHP;

        float progress = (float)currentHP / (float)maxHP;
        auto progressText = "HP: " + std::to_string(currentHP) + "/" + std::to_string(maxHP);

        linksDivider = 15;
        modelTransform = &HeroUITransform;
        uiFunc = NULL;
        nearPlaneClip = -2000;
        modelProject = &fightProject;

        
        const float barWidth = 4;
        const float barHeight = .25;
        const float barX = -barWidth/2;
        const float barY = -barHeight/2+2.5;
        const int starCount = 20;
        const int starSize = 5;
        
       
        point3d topLeft = { barX, barY, 0 };
        point3d topRight = { barX + barWidth, barY, 0 };
        point3d bottomLeft = { barX, barY + barHeight, 0 };
        point3d bottomRight = { barX + barWidth, barY + barHeight, 0 };

        modelTransform(topLeft, *playerConstellation);
        modelTransform(topRight, *playerConstellation);
        modelTransform(bottomLeft, *playerConstellation);
        modelTransform(bottomRight, *playerConstellation);

       
        drawLine(topLeft, topRight, 50);    // ����
        drawLine(bottomLeft, bottomRight, 50); // ���
        drawLine(topLeft, bottomLeft, 5);  // ����
        drawLine(bottomRight, topRight,  5); // �����

        // ������ ���������� �� ����
        int activeStars = (int)(starCount * progress);
        float starSpacing = barWidth / (float)(starCount + 1);

        for (int i = 0; i < starCount; i++) {
            point3d star = {
                barX + (i + 1) * starSpacing,
                barY + barHeight / 2,
                0
            };

            float currentSize = (i < activeStars) ? starSize * 1.5f : starSize*.0;

            if (i < activeStars) {
                float pulse = 0.5f + 0.5f * sinf(currentTime * 0.005f);
                currentSize += pulse * 2.0f;
            }

            modelTransform(star, *playerConstellation);
            modelProject(star);

            star.draw(star, currentSize);
        }

        // ��������� �����
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

            modelTransform(star1, *playerConstellation);
            modelTransform(star2, *playerConstellation);

            int lineSegments = (i < activeStars - 1) ? 10 : 5;
           drawLine(star1, star2, lineSegments);
        }

        point3d p = { 0,2,0 };
        modelTransform(p, *playerConstellation);
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
            InitEnemyOrbit();
            //TextOutA(window.context, 400, 400, "��� �������", 10);
            drawString("Start Fight", 400, 400, 1 ,true);
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
            if (!isRewindActive) {
                // �������� ����� ������ ���� ���� ���� ������������
                if (currentStateIndex > (hasAnchorPoint ? anchorIndex : 0)) {
                    isRewindActive = true;
                    RewindOneStepBack();
                }
            }
            else {
                // ���������� �����
                if (currentStateIndex > (hasAnchorPoint ? anchorIndex : 0)) {
                    RewindOneStepBack();
                }
            }
        }
        else if (isRewindActive) {
            // ��������� �����
            isRewindActive = false;
            ResetTimeAnchor();
            needInitialAnchor = true; // ��������� � ������ �����
        }
    
        if (isBattleActive) {
            LONG remainingTime = (LONG)((battleStartTime + battleTime + timeModifier) - currentTime);
           
            DWORD totalBattleTime = battleTime + timeModifier;
    
            if (totalBattleTime > MAX_BATTLE_TIME) {
                timeModifier = MAX_BATTLE_TIME - battleTime;
                remainingTime = (LONG)((battleStartTime + MAX_BATTLE_TIME) - currentTime);
            }
    
            if (remainingTime > 0) {
                
                
                std::string timeStr = "Time: " + std::to_string(remainingTime / 1000);
                drawString(timeStr.c_str(), window.width / 1.1, 45, 1.f, true);
    
            }
            else 
            {
               timeModifier = 0;
               isBattleActive = false;
               gameState = gameState_::EndFight;
              
            }
        }
    }

    

    void drawQuest()
    {
        point3d point[4];

        PVFN Quest[4][4] = {
            {&StarQuestUi, NULL, NULL, NULL},
            {&StarFightUi, NULL, NULL, NULL},
            {&StarRiddleUi, NULL, NULL, NULL},
            {&StarQuestUi, NULL, NULL, NULL}
        };
        
        srand(10);
        for (int i = 0; i < 4; i++)
        {            
            genWaySphere(point[i]);
            modelTransform(point[i], Aries);
            modelProject(point[i]);
            int starSize = 10;
            
            point[i].draw(point[i], starSize);
            Quest[i][0](point[i]);
        }
        
    }
    void drawBook()
    {
        const float barWidth = 1200;
        const float barHeight = 650;
        const float barX = 10;
        const float barY = 10;
        const float starCount = 10;
        const float starSize = 10.0f;

        point3d Bar = { barX, barY,0 };

        // ������� ����� ��� �����
        point3d topLeft = { Bar.x - barWidth , Bar.y - barHeight, 0 };
        point3d topRight = { Bar.x + barWidth , Bar.y - barHeight, 0 };
        point3d bottomLeft = { Bar.x - barWidth, Bar.y + barHeight, 0 };
        point3d bottomRight = { Bar.x + barWidth, Bar.y + barHeight, 0 };

        
        // ������ �����
        drawLine(topLeft, topRight, 50);    // ����
        drawLine(bottomLeft, bottomRight, 50); // ���
        drawLine(topLeft, bottomLeft, 20);  // ����
        drawLine(bottomRight, topRight, 20); // �����

    }
 
    void drawWorld()
    {
        drawBack();
        nearPlaneClip = 0;

        SelectObject(window.context,mainBrush);
        SelectObject(window.context, mainPen);

        
        textStyle.color = RGB(0, 191, 255);

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
                drawPlayer�onstellationToMenu(entities[player_sign]);
                menuMonthprocessing();
                menuDayprocessing();
                menuConfirmationButton();
                break;

            case gameState_::selectEnemy:
            {
                modelTransform = &placeToWorld;
                modelProject = &fightProject;
                uiFunc = &constSelectUI;
                linksDivider = 50;
                drawStarField();
                drawMilkyWay();
                drawQuest();
                modelTransform = &placeConstToWorld;

                for (int i = 0; i < 1; i++)
                {
                    draw�onstellation(entities[i]);
                }

                std::string curentSignstring = zodiacSignToString(player_sign);
                //TextOutA(window.context, window.width * 5 / 6, window.height-window.height/20., curentSignstring.c_str(), curentSignstring.size());
                drawString(curentSignstring.c_str(), window.width * 5 / 6, window.height - window.height / 20., 1, false);

                drawString("Find Constallations and click on him", window.width/2, (200./1440)*window.height, 1, true);
                drawString("Features:\nMouse wheel to zoom in and out", (1700./2560)*window.width,(1200./1440)*window.height , .7f, false);
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
                
                textStyle.color = elements[(int)current_element].color;
                DrawCombatStats();
                textStyle.color = RGB(0, 191, 255);
               
                if (attackCooldown == true) {

                    AttackVector();

                    if (current_weapon == weapon_name::Shield)
                    {
                        drawShieldCircle();
                    }
                }
                             
                StartBattle();
                enemyFight();

                DrawHpHeroBar(entities[player_sign]);
                modelTransform = &placeToWorld;
                modelProject = &fightProject;  
                uiFunc = &starIntersectUI;
                linksDivider = 50;
                drawStarField();

                UpdateEnemyOrbitPosition();
                
                modelTransform = &placeConstToWorld;
                nearPlaneClip = 0;
                
                srand(currentTime);

                DrawHpEnemyBar(entities[currentEnemyID]);
                modelTransform = &placeConstToWorld;//����
                
                nearPlaneClip = 0;
                if (isShakingHero) {

                    float beamTime = 4.*(currentTime - shakeStartTimeHero) / shakeDurationHero;
                    if (beamTime > 1.) beamTime = 1;
                    for (int i = 0;i < starSet[currentEnemyID]->starsCords.size();i++)
                    {
                        auto p1 = starSet[currentEnemyID]->starsCords[i];
                        point3d p2 = starSet[player_sign]->starsCords[i% starSet[player_sign]->starsCords.size()];

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
                    
                    draw�onstellation(entities[currentEnemyID]);

                    linksDivider = 15;
                    modelTransform = &placeHeroToWorld;
                    uiFunc = &heroUI;
                    nearPlaneClip = -2000;
                    SelectObject(window.context, heroBrush);
                    SelectObject(window.context, heroPen);

                    draw�onstellation(entities[player_sign], true);//�����

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
                    draw�onstellation(entities[currentEnemyID]);
                }

                if (currentTime > attack_time + weapon[(int)current_weapon].attackSpeed and attack_start == true)
                {
                    isDamageTaken = true;
                    attack_start = false;
                }

                SelectObject(window.context, mainBrush);
                SelectObject(window.context, mainPen);

                modelTransform = &placeConstToWorld;
                DrawStarsHP(window.context, entities[currentEnemyID]);

                
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
                if (!GetAsyncKeyState(VK_LBUTTON)) 
                {
                    
                }
                SelectObject(window.context, heroBrush);
                SelectObject(window.context, heroPen);

                draw�onstellation(entities[player_sign], true);

                SelectObject(window.context, mainBrush);
                SelectObject(window.context, mainPen);


                
                float cdTimeOut =1.- (currentTime - attack_cooldown) / 5000.;
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
            case gameState_::Helper:
            {
                drawBook();
                Helpers();

                break;
            }
            case gameState_::EndFight:
            { 
                loseFight();

                break;
            }
            case gameState_::Quest:
            {
                drawStarField();
                drawString("Hello, I am the cosmic Aries Flea: Solve my riddle:\nThe thick grasses are entwined,\nThe meadows are curled,\nAnd I myself am all curly,\nEven the curl of my horn.", window.width/2, window.height/2,1.,true);
                QuestExit();
                break;
            }
            case gameState_::Riddle:
            {
                drawStarField();
                QuestExit();

                break;
            }
            case gameState_::StarFight: 
            {
                drawStarField();
                QuestExit();

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

