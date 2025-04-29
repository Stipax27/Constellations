namespace drawer
{
        
    void (*modelTransform)(point3d& p, Constellation& Constellation);
    void (*modelProject)(point3d& p);


    void placeConstToWorld(point3d& p, Constellation& Constellation)
    {
        Shaking(p);

        move(p, 0, 0, 3000. / Constellation.scale);
        rotateX(p, Constellation.angle.x);
        rotateY(p, Constellation.angle.y);
        rotateZ(p, Constellation.angle.z);
        p.x *= Constellation.scale;
        p.y *= Constellation.scale;
        p.z *= Constellation.scale;

        rotateX(p, mouseAngle.y * 0.1);
        rotateY(p, mouseAngle.x * 0.1);
    }

    void placeWeaponToWorld(point3d& p, Constellation& Constellation)
    {
        move(p, 0, 0, 3000. / Constellation.scale);
        rotateX(p, Constellation.angle.x);
        rotateY(p, Constellation.angle.y);
        rotateZ(p, Constellation.angle.z);
        p.x *= Constellation.scale;
        p.y *= Constellation.scale;
        p.z *= Constellation.scale;

        rotateX(p, mouseAngle.y * 0.1);
        rotateY(p, mouseAngle.x * 0.1);
    }

    void placeConstToStartMenu(point3d& p, Constellation& Constellation)
    {
        p.x *= 200;
        p.y *= 200;
        p.z *= 200;
        //move(p, offset.x, offset.y, offset.z);
        float a = currentTime;
        rotateY(p, a * 0.1);
        move(p, 0, 0, 1300);
    }

    void placeToWorld(point3d& p, Constellation& Constellation)
    {
        rotateX(p, mouseAngle.y * 0.1);
        rotateY(p, mouseAngle.x * 0.1);
    }

    void placeHeroToWorld(point3d& p, Constellation& Constellation)
    {
        ShakingHero(p);
        move(p, 0, 0, 0. / Constellation.scale);

        fightMove(p);

        p.x *= .13;
        p.y *= .13;
        p.z *= .13;
        //rotateX(p, Constellation.angle.x);
        //rotateY(p, Constellation.angle.y);
        //rotateZ(p, Constellation.angle.z);
        p.x *= Constellation.scale;
        p.y *= Constellation.scale;
        p.z *= Constellation.scale;

        rotateX(p, mouseAngle.y * 0.1);
        rotateY(p, mouseAngle.x * 0.1);
    }
    
    void HeroUITransform(point3d& p, Constellation& Constellation)
    {
        move(p, 0, 0, 0.);

        fightMove(p);

        p.x *= .13;
        p.y *= .13;
        p.z *= .13;
        //rotateX(p, Constellation.angle.x);
        //rotateY(p, Constellation.angle.y);
        //rotateZ(p, Constellation.angle.z);
        p.x *= Constellation.scale;
        p.y *= Constellation.scale;
        p.z *= Constellation.scale;

        //rotateX(p, mouseAngle.y * 0.1);
        //rotateY(p, mouseAngle.x * 0.1);
    }

    void fightProject(point3d& p)
    {
        int fadeInTime = currentTime - startTime;
        float startCamDist = 100;
        float finCamDist = 3000;

        float lerpCoef = smoothstep(0., 1, min(fadeInTime * 4, finCamDist) / (finCamDist - startCamDist));
        if (lerpCoef < .99) {
            camDist = lerpCoef * (finCamDist - startCamDist) + startCamDist;
        }
        camDist = clamp(camDist, 100, 3000);
        float x = window.width / 2. + p.x * camDist / (p.z + camDist);
        float y = window.height / 2. + p.y * camDist / (p.z + camDist);

        p.x = x;
        p.y = y;
    }

    void menuProject(point3d& p)
    {
        float camDist = 30000;
        float x = window.width / 2. + p.x * camDist / (p.z + camDist);
        float y = window.height / 2. + p.y * camDist / (p.z + camDist);
        p.x = x;
        p.y = y;
    }

    void projectSingleConst(point3d& p)
    {
        float camDist = 100;

        float x = window.width / 2. + 1000*p.x * camDist / (p.z + camDist);
        float y = window.height / 2. + 1000*p.y * camDist / (p.z + camDist);

        p.x = x;
        p.y = y;
    }
    void NullProject(point3d& p) {
    
    }

    void genRandSphere(point3d& p)
    {
        float amp = 1.25;
        float angleX, angleY;
        angleX = rand() % 360;
        angleY = rand() % 360;

        p.x = 0;
        p.y = 0;
        p.z = window.width;

        rotateX(p, angleX);
        rotateY(p, angleY); // Ñôåðîîáðàçíîå ïðîñòðàíñòâî.

    }

    float nearPlaneClip = 0;

    void drawPoint(point3d& p, float sz = 2)
    {
        if (p.z < nearPlaneClip) return;

        Ellipse(window.context,
            p.x - sz,
            p.y - sz,
            p.x + sz,
            p.y + sz
        );// Ðèñîâàíèå ýëèïñà. sz = Ðàçìåð çâåçäû.

    }

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
            point.x = p1.x + dx * (float)i/(float)(count-1);
            point.y = p1.y + dy * (float)i/(float)(count-1);
            point.z = p1.z + dz * (float)i/(float)(count-1);

            modelProject(point);

            float sz = 1 + .5 * sinf(i + currentTime * .01);
            drawPoint(point, sz);
            // Ðèñîâàíèå Ëèíèé.
        }
    }

    

    
    float linksDivider = 25;

    void drawLinks(Constellation& Constellation)
    {
        //std::vector <point3d>& starArray = Constellation.starsRenderedCords;
        std::vector <point3d>& starArray = Constellation.starsCords;
        std::vector<std::vector<float>>& starEdges = Constellation.constellationEdges;
        std::vector <float>& starHealth = Constellation.starsHealth;

        int starsEdgesCount = starEdges.size();
        for (int i = 0; i < starsEdgesCount; i++)
        {
            point3d point1, point2;
            point1.x = starArray[starEdges[i][0]].x;
            point1.y = starArray[starEdges[i][0]].y;
            point1.z = starArray[starEdges[i][0]].z;

            point2.x = starArray[starEdges[i][1]].x;
            point2.y = starArray[starEdges[i][1]].y;
            point2.z = starArray[starEdges[i][1]].z;

            float a = currentTime * .01;
            modelTransform(point1, Constellation);
            modelTransform(point2, Constellation);
            //if (starHealth[i] > 0 && starHealth[i + 1] > 0) - Áûëî

            if (starHealth[starEdges[i][0]] > 0 && starHealth[starEdges[i][1]] > 0) // - Ñòàëî
            {

                float dx = point2.x - point1.x;
                float dy = point2.y - point1.y;
                float dz = point2.z - point1.z;
                float length = sqrt(dx * dx + dy * dy + dz * dz);
                int x = static_cast<int>(length) / linksDivider;
                drawLine(point1, point2, x);// Ðèñîâàíèå çâ¸çäíûõ ëèíèé ñîçâåçäèÿ.
            }
        }
    }
    
    HBRUSH brush;
    HBRUSH brush2;
    float finalStarRad = 0;

    void constSelectUI(point3d& point, Constellation& Constellation, int i)
    {
        std::vector <float>& starHealth = Constellation.starsHealth;

        float dx = point.x - mouse.x;
        float dy = point.y - mouse.y;
        float lenght = sqrt(dx * dx + dy * dy);

        float rad = saturate(1.2 - lenght * .05) * fabs(sin(currentTime * .01));


        if (GetAsyncKeyState(VK_LBUTTON))
        {
            if (lenght < starSize)
            {
                SelectObject(window.context, brush2);
                gameState = gameState_::Fight;
                currentEnemy = &Constellation;
                currentEnemyID = (ZodiacSign)(currentEnemy->ID);
            }
        }
        else
        {
            SelectObject(window.context, brush);
        }

        finalStarRad = starSize * starHealth[i] + rad * 15;
    }

    float get_lenghts(point3d& point1, point3d& point2)
    {
    
        float a = point1.x - point2.x;
        float b = point1.y - point2.y;
        float c = sqrt(a * a + b * b);
        return c;

    }

    float line_hit;
    float coub_hit;
    bool check_attack = true;
    bool attack_collision = false;

    void starIntersectUI(point3d& point, Constellation& Constellation, int i)
    {
        std::vector <float>& starHealth = Constellation.starsHealth;

        float dx = point.x - mouse.x;
        float dy = point.y - mouse.y;
        float lenght = sqrt(dx * dx + dy * dy);

        float rad = saturate(1.2 - lenght * .05) * fabs(sin(currentTime * .01));
        finalStarRad = starSize * starHealth[i] + rad * 15;



        if (current_weapon == weapon_name::Sword)
        {

            float line_x = get_lenghts(attack[0], attack[1]);
            float line_y = get_lenghts(attack[0], point);
            float line_z = get_lenghts(attack[1], point);
            float line_yz = line_z + line_y;
            line_hit = line_yz / line_x;

            if (GetAsyncKeyState(VK_LBUTTON))
            {
                if (line_hit < 1.001)
                {
                    attack_collision = true;
                }
                SelectObject(window.context, brush);
            }
            else
            {
                if (line_hit < 1.01 and check_attack == false)
                {

                    SelectObject(window.context, brush2);
                    starHealth[i] -= weapon[(int)current_weapon].damage;
                    isDamageTaken = true;
                }
            }

        }


        if (current_weapon == weapon_name::Shield)
        {

            float centerX = (mouse.x + oldmouse.x) / 2;
            float centerY = (mouse.y + oldmouse.y) / 2;

            float dx = mouse.x - oldmouse.x;
            float dy = mouse.y - oldmouse.y;
            float shieldRadius = sqrt(dx * dx + dy * dy);

            float stardx = point.x - centerX;
            float stardy = point.y - centerY;
            float distToCenter = sqrt(stardx * stardx + stardy * stardy);

            if (GetAsyncKeyState(VK_LBUTTON))
            {
                if (distToCenter <= shieldRadius)
                {
                    attack_collision = true;

                }

                SelectObject(window.context, brush);
            }
            else
            {
                if (distToCenter <= shieldRadius and check_attack == false)
                {
                    SelectObject(window.context, brush2);
                    starHealth[i] -= weapon[(int)current_weapon].damage;
                    isDamageTaken = true;
                }
            }
        }

        if (current_weapon == weapon_name::Bow)
        {

            float dx = point.x - oldmouse.x;
            float dy = point.y - oldmouse.y;
            float distToStart = sqrt(dx * dx + dy * dy);
            float hitRadius = 20;

            if (GetAsyncKeyState(VK_LBUTTON))
            {
                if (distToStart <= hitRadius)
                {
                    attack_collision = true;
                }

                SelectObject(window.context, brush);
            }
            else
            {
                if (distToStart <= hitRadius and check_attack == false)
                {
                    SelectObject(window.context, brush2);
                    starHealth[i] -= weapon[(int)current_weapon].damage;
                    isDamageTaken = true;
                }
            }
        }

    }

    void starUI(point3d& point, Constellation& Constellation, int i)
    {
        std::vector <float>& starHealth = Constellation.starsHealth;

        float dx = point.x - mouse.x;
        float dy = point.y - mouse.y;
        float lenght = sqrt(dx * dx + dy * dy);

        float rad = saturate(1.2 - lenght * .05) * fabs(sin(currentTime * .01));


        if (GetAsyncKeyState(VK_LBUTTON))
        {
            if (lenght < starSize)
            {
                SelectObject(window.context, brush2);
                starHealth[i] -= .1;

            }
        }
        else
        {
            SelectObject(window.context, brush);
        }

        finalStarRad = starSize * starHealth[i] + rad * 15;
        
    }

    void heroUI(point3d& point, Constellation& Constellation, int i)
    {
        std::vector <float>& starHealth = Constellation.starsHealth;

        float dx = point.x - mouse.x;
        float dy = point.y - mouse.y;
        float lenght = sqrt(dx * dx + dy * dy);

        float rad = saturate(1.2 - lenght * .05) * fabs(sin(currentTime * .01));


        if (GetAsyncKeyState(VK_LBUTTON))
        {
            if (lenght < starSize)
            {
                SelectObject(window.context, brush2);
            }
        }
        else
        {
            SelectObject(window.context, brush);
        }
        
        finalStarRad = 3 * starHealth[i] + rad * 15;
        
    }

    void menuUI(point3d& point, Constellation& Constellation, int i)
    {
        finalStarRad = 5;
    }

    void (*uiFunc)(point3d& point, Constellation& Constellation, int i);

    void drawStarPulse(Constellation& Constellation)
    {
        std::vector <point3d>& starArray = Constellation.starsCords;
        std::vector <float>& starHealth = Constellation.starsHealth;

        int starsCount = starArray.size();
        brush = CreateSolidBrush(RGB(0, 191, 255));
        brush2 = CreateSolidBrush(RGB(255, 0, 0));
        SelectObject(window.context, brush);
        for (int i = 0; i < starsCount; i++)
        {
            point3d point;
            point.x = starArray[i].x;
            point.y = starArray[i].y;
            point.z = starArray[i].z;

            float a = currentTime * .01;
            modelTransform(point, Constellation);
            modelProject(point);

            // Ïóëüñèðîâàíèå Çâ¸çä ïðè íàâåäåíèå ìûøè.
            finalStarRad = 1;
            if (uiFunc)
            {
                uiFunc(point, Constellation, i);
            }

            if (finalStarRad > 0)
            {
                drawPoint(point, finalStarRad);
            }

        }
        DeleteObject(brush);
        DeleteObject(brush2);
    }

    

    void drawÑonstellation(Constellation& Constellation)
    {
        drawLinks(Constellation);
        drawStarPulse(Constellation);
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
            rotateZ(point, starfield_angles.z);
            modelProject(point);



            drawPoint(point,.5);
            // Çâ¸çäû íà ôîíå èõ êîë-âî. è Êàäð îñòàíîâêè.
        }
    }

    const COLORREF colors[] =
    {
        RGB(255, 0, 0),    // Êðàñíûé
        RGB(255, 165, 0),  // Îðàíæåâûé
        RGB(255, 255, 0),  // Æåëòûé
        RGB(0, 255, 0),    // Çåëåíûé
        RGB(0, 255, 255),  // Ãîëóáîé
        RGB(0, 0, 255),    // Ñèíèé
        RGB(128, 0, 128)   // Ôèîëåòîâûé
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

            DeleteObject(brush);// Äàííûå äëÿ ðèñîâàíèå öâåò. êðóãà.
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
            mouse.x - starSize,
            mouse.y - starSize,
            mouse.x + starSize,
            mouse.y + starSize
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
        auto blackBrush = CreateSolidBrush(RGB(0, 0, 0));
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
                morphArray.push_back(lerp(starArray1[i], starArray2[i], (0.5 + 0.5 * sin(currentTime * morphSpeed))));
            }
        }
        else
        {
            for (int i = 0; i < sz2;i++)
            {
                float morphSpeed = 0.01;
                morphArray.push_back(lerp(starArray1[i], starArray2[i], (0.5 + 0.5 * sin(currentTime * morphSpeed))));
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
        
        //drawÑonstellation(morphArray, Morp_indices, Morp_health); Îòêëþ÷åíî
    }

    void drawPlayerÑonstellationToMenu()
    {
        startTime = currentTime;
        int n = (currentTime / 1000) % starSet.size();
        modelTransform = &placeConstToStartMenu;
        modelProject = &menuProject;
        uiFunc = &menuUI;
        linksDivider = 15;
        if (gameState == gameState_::confirmSign) n = player_sign;
        //drawÑonstellation(*starSet[n]);

        drawÑonstellation(*starSet[player_sign]);
        
    }

    void enemyFight()
    {
                float e = 1000;
        if (currentTime > attackTime + e)
        {
            attackTime = currentTime;
            enemyAttack(*starSet[player_sign]);

        }
        
    if (getConstellationHP(*starSet[player_sign]) <=0)
        {
            gameState == gameState_::EndFight;
        }
    }

    void drawShieldCircle() 
    {

        float dx = mouse.x - oldmouse.x;
        float dy = mouse.y - oldmouse.y;
        float shieldRadius = sqrt(dx * dx + dy * dy);

        // Öåíòð = oldmouse (êðóã ðàñò¸ò èç ýòîé òî÷êè)
        float centerX = oldmouse.x;
        float centerY = oldmouse.y;
        modelProject = &NullProject;
        if (GetAsyncKeyState(VK_LBUTTON)) {
            for (int i = 0; i < 36; i++) {
                float angle = i * (2 * PI / 36);  // 36 òî÷åê äëÿ ãëàäêîãî êðóãà
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


    void SelectVectorAttack()
    {

        if (current_weapon == weapon_name::Sword)
        {

            AttackSwordVector();

        }

        if (current_weapon == weapon_name::Shield)
        {

            AttackShieldVector();
            drawShieldCircle();

        }

        if (current_weapon == weapon_name::Bow)
        {

            AttackBowVector();

        }
        if (current_weapon == weapon_name::Staff)
        {

        }

    }

    void DrawStarsHP(HDC hdc) {

        for (size_t i = 0; i < starSet[currentEnemyID]->starsCords.size(); ++i) {
            auto pos = starSet[currentEnemyID]->starsCords[i];
            float hp = starSet[currentEnemyID]->starsHealth[i];

            modelTransform(pos, *starSet[currentEnemyID]);
            modelProject(pos);

            if (hp>0)
            {
                std::string hpText = "HP: " + std::to_string(hp);
                drawString(hpText.c_str(), pos.x, pos.y - 40, .5, true, false, 7);
            }
        }
    }

    void DrawTimeBar() 
    {
    auto player_const = *starSet[player_sign];
    auto maxHP = player_const.maxHP;
    auto progress = getConstellationHP(*starSet[player_sign])/ maxHP;
    auto progressText = "HP: " + std::to_string(progress);


    

    linksDivider = 15;
    modelTransform = &HeroUITransform;
    uiFunc = NULL;
    nearPlaneClip = -2000;
    modelProject = &fightProject;;

    // Ïàðàìåòðû âðåìåííîãî áàðà
    const float barWidth = 4;
    const float barHeight = .25;
    const float barX = -barWidth/2;
    const float barY = -barHeight/2+2.5;
    const float starCount = 20;
    const float starSize = 5.0f;
    
    // Ñîçäàåì òî÷êè äëÿ ðàìêè
    point3d topLeft = { barX, barY, 0 };
    point3d topRight = { barX + barWidth, barY, 0 };
    point3d bottomLeft = { barX, barY + barHeight, 0 };
    point3d bottomRight = { barX + barWidth, barY + barHeight, 0 };

    modelTransform(topLeft, player_const);
    modelTransform(topRight, player_const);
    modelTransform(bottomLeft, player_const);
    modelTransform(bottomRight, player_const);

    // Ðèñóåì ðàìêó
    drawer::drawLine(topLeft, topRight, 50);    // Âåðõ
    drawer::drawLine(bottomLeft, bottomRight, 50); // Íèç
    drawer::drawLine(topLeft, bottomLeft, 5);  // Ëåâî
    drawer::drawLine(bottomRight, topRight,  5); // Ïðàâî

    

    // Ðèñóåì çàïîëíåíèå èç çâ¸çä
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

        modelTransform(star, player_const);
        modelProject(star);

        drawer::drawPoint(star, currentSize);
    }

    // Ñîåäèíÿåì çâ¸çäû
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
        TextOutA(window.context, 400, 400, "Áîé íà÷àëñÿ", 10);
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
            
            DrawTimeBar();
            std::string timeStr = "Time: " + std::to_string(remainingTime / 1000);
            drawString(timeStr.c_str(), window.width / 2, 10, 1.f, true);

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
 point3d startPoint; 

void FlightWeapons(point3d& p, Constellation& Constellations)
{

    point3d enemyPosition = { 0,0,0 };
            //starSet[currentEnemyID]->getPosition();

    if(weapon[(int)current_weapon].constellation)
    {
        p = weapon[(int)current_weapon].constellation->getPosition();
        
        //drawÑonstellation(*weapon[(int)current_weapon].constellation);
         
        /*point3d direction = enemyPosition - p;*/

        startPoint = { 0, 0, 0 };
        placeHeroToWorld(startPoint, *starSet[player_sign]);

        int ñount = 100;
        placeConstToWorld(enemyPosition, *starSet[currentEnemyID]);
        modelProject = &fightProject;

        drawLine(startPoint, enemyPosition, ñount); 

        /*float distance = direction.length();*/
        
        /*if (distance > 0) {
            direction = direction * (1.0f / distance);


            p.x += direction.x * speed;
            p.y += direction.y * speed;
            p.z += direction.z * speed;


            if (distance <= speed) {

            }*/
        

    }
}
//void drawWeapon(point3d& p,Constellation& Constellations) 
//{
//    srand(10);
//    for (int i = 0; i < 1; i++)
//    {
//       float rand()
//        
//    drawÑonstellation(*weapon[(int)current_weapon].constellation);
//    }
//}




    

    

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
                //drawPlayerÑonstellationToMenu();
                menuMonthprocessing();
                break;

            case gameState_::DaySelection:
                //drawPlayerÑonstellationToMenu();
                menuMonthprocessing();
                menuDayprocessing();
                break;

            case gameState_::confirmSign:
                drawPlayerÑonstellationToMenu();
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

                modelTransform = &placeConstToWorld;

                for (int i = 0;i < starSet.size();i++)
                {
                    drawÑonstellation(*starSet[i]);
                }

                std::string curentSignstring = zodiacSignToString(player_sign);
                TextOutA(window.context, window.width * 5 / 6, window.height-window.height/20., curentSignstring.c_str(), curentSignstring.size());

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
                SelectVectorAttack();
                StartBattle();
                enemyFight();
                

                modelTransform = &placeToWorld;
                modelProject = &fightProject;
                uiFunc = &starIntersectUI;
                linksDivider = 50;
                drawStarField();

                
                modelTransform = &placeWeaponToWorld;
                nearPlaneClip = 0;
                //drawÑonstellation(*weapon[(int)current_weapon].constellation);


                srand(currentTime);


                modelTransform = &placeConstToWorld;
                //drawÑonstellation(*currentEnemy);
                nearPlaneClip = 0;

                
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


                if (!GetAsyncKeyState(VK_LBUTTON))
                {
                    if (attack_collision == true)
                    {
                        check_attack = false;
                     }
                    //updateConstellation(starsCords);

                    drawÑonstellation(*starSet[currentEnemyID]);

                    if (attack_collision == true)
                    {
                        check_attack = true;
                        attack_collision = false;
                    }

                    
                }
                else
                {
                    drawÑonstellation(*starSet[currentEnemyID]);

                    //check_attack = false;
                }
                
                SelectObject(window.context, mainBrush);
                SelectObject(window.context, mainPen);


                DrawStarsHP(window.context);
                FlightWeapons(startPoint,*starSet[currentEnemyID]);

                linksDivider = 15;
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

                drawÑonstellation(*starSet[player_sign]);

                

               
                std::string curentSignstring = zodiacSignToString(currentEnemyID);
                drawString(curentSignstring.c_str(), window.width / 2, window.height / 20., 1, true);

                curentSignstring = zodiacSignToString(player_sign);
                drawString(curentSignstring.c_str(), window.width / 2, window.height - window.height / 15., 1, true);

                curentSignstring = "current weapon: " + weapon[(int)current_weapon].name;
                drawString(curentSignstring.c_str(), window.width / 2, window.height - window.height / 30., 1, true);


                UpdateGame();
                


                //drawColorCircle();

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

