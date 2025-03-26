namespace drawer
{
        
    void (*modelTransform)(point3d& p, Constellation& Constellation);
    void (*modelProject)(point3d& p);

    void rotateworld(point3d& p)
    {
        rotateX(p, mouseAngle.y * 0.1);
        rotateY(p, mouseAngle.x * 0.1);
        //rotateZ(p, timeGetTime() * 0.01); // Êðó÷åíèå îáüåêòîâ.
    }

    void placeConstToWorld(point3d& p, Constellation& Constellation)
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
        float a = timeGetTime();
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
        move(p, 0, 0, 0. / Constellation.scale);
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
    
    void fightProject(point3d& p)
    {
        int currentTime = timeGetTime() - startTime;
        float startCamDist = 100;
        float finCamDist = 3000;

        float lerpCoef = smoothstep(0., 1, min(currentTime * 4, finCamDist) / (finCamDist - startCamDist));
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
        for (int i = 0;i < count;i++)
        {
            float dx = p2.x - p1.x;
            float dy = p2.y - p1.y;
            float dz = p2.z - p1.z;
            float length = sqrt(dx * dx + dy * dy + dz * dz);
            float stepX = dx / (float)count;
            float stepY = dy / (float)count;
            float stepZ = dz / (float)count;

            point3d point;
            point.x = p1.x + stepX * (float)i;
            point.y = p1.y + stepY * (float)i;
            point.z = p1.z + stepZ * (float)i;

            modelProject(point);

            float sz = 1 + .5 * sinf(i + timeGetTime() * .01);
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

            float a = timeGetTime() * .01;
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

        float rad = saturate(1.2 - lenght * .05) * fabs(sin(timeGetTime() * .01));


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

    void starUI(point3d& point, Constellation& Constellation, int i)
    {
        std::vector <float>& starHealth = Constellation.starsHealth;

        float dx = point.x - mouse.x;
        float dy = point.y - mouse.y;
        float lenght = sqrt(dx * dx + dy * dy);

        float rad = saturate(1.2 - lenght * .05) * fabs(sin(timeGetTime() * .01));


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

        float rad = saturate(1.2 - lenght * .05) * fabs(sin(timeGetTime() * .01));


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

            float a = timeGetTime() * .01;
            modelTransform(point, Constellation);
            modelProject(point);

            // Ïóëüñèðîâàíèå Çâ¸çä ïðè íàâåäåíèå ìûøè.
            finalStarRad = 1;
            uiFunc(point, Constellation, i);

            if (finalStarRad > 0)
            {
                drawPoint(point, finalStarRad);
            }

        }
        DeleteObject(brush);
        DeleteObject(brush2);
    }

    void drawStarPulse22(Constellation& Constellation)
    {
        SelectVector();
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

            //float a = timeGetTime() * .01;
            modelTransform(point, Constellation);
            modelProject(point);

            // Ïóëüñèðîâàíèå Çâ¸çä ïðè íàâåäåíèå ìûøè.
            finalStarRad = 1;
            uiFunc(point, Constellation, i);

            if (finalStarRad > 0)
            {
                drawPoint(point, finalStarRad);
            }

            if (point.x < mouse.x - oldmouse.x and point.y < mouse.x - oldmouse.y)
            {

                SelectObject(window.context, brush2);

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
        srand(10);
        for (int i = 0; i < 2000; i++)
        {
            point3d point;
            genRandSphere(point);
            modelTransform(point,Aries);
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
                morphArray.push_back(lerp(starArray1[i], starArray2[i], (0.5 + 0.5 * sin(timeGetTime() * morphSpeed))));
            }
        }
        else
        {
            for (int i = 0; i < sz2;i++)
            {
                float morphSpeed = 0.01;
                morphArray.push_back(lerp(starArray1[i], starArray2[i], (0.5 + 0.5 * sin(timeGetTime() * morphSpeed))));
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
        startTime = timeGetTime();
        int n = (timeGetTime() / 1000) % starSet.size();
        modelTransform = &placeConstToStartMenu;
        modelProject = &menuProject;
        uiFunc = &menuUI;
        linksDivider = 15;
        if (gameState == gameState_::confirmSign) n = player_sign;
        //drawÑonstellation(*starSet[n]);
        drawÑonstellation(*starSet[player_sign]);
    }


    void drawWorld()
    {
        
        drawBack();
        nearPlaneClip = 0;

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

            case gameState_::Fight:
            {
                
                SelectVector();

                StartBattle();
                

                modelTransform = &placeToWorld;
                modelProject = &fightProject;
                uiFunc = &starUI;
                linksDivider = 50;
                drawStarField();

                modelTransform = &placeConstToWorld;


                //drawÑonstellation(*currentEnemy);
                nearPlaneClip = 0;
                drawÑonstellation(*starSet[currentEnemyID]);

                linksDivider = 15;
                modelTransform = &placeHeroToWorld;
                uiFunc = &heroUI;
                nearPlaneClip = -2000;
                drawÑonstellation(*starSet[player_sign]);


                std::string curentSignstring = zodiacSignToString(currentEnemyID);
                TextOutA(window.context, window.width / 2, window.height / 20., curentSignstring.c_str(), curentSignstring.size());

                curentSignstring = zodiacSignToString(player_sign);
                TextOutA(window.context, window.width / 2, window.height - window.height / 20., curentSignstring.c_str(), curentSignstring.size());

                UpdateGame();
                //drawColorCircle();

                break;
            }

        }

        drawGameCursor();

    }
}

