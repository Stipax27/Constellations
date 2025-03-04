namespace drawer
{
    void rotateworld(point3d& p)
    {
        rotateX(p, mouseAngle.y * 0.1);
        rotateY(p, mouseAngle.x * 0.1);
        //rotateZ(p, timeGetTime() * 0.01); // Êğó÷åíèå îáüåêòîâ.
    }
    
    void project(point3d& p)
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
        rotateY(p, angleY); // Ñôåğîîáğàçíîå ïğîñòğàíñòâî.
    }


    void drawPoint(point3d& p, float sz = 2)
    {
        if (p.z < 0) return;

        Ellipse(window.context,
            p.x - sz,
            p.y - sz,
            p.x + sz,
            p.y + sz
        );// Ğèñîâàíèå ıëèïñà. sz = Ğàçìåğ çâåçäû.

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

            project(point);

            float sz = 1 + .5 * sinf(i + timeGetTime() * .01);
            drawPoint(point, sz);
            // Ğèñîâàíèå Ëèíèé.
        }
    }
    //void drawLinks(std::vector <point3d>& starArray, std::vector<std::vector<float>> starEdges, std::vector <float>& starHealth)

    void drawLinks(Constellation& Constellation)
    {
        std::vector <point3d>& starArray = Constellation.starsRenderedCords;
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
            rotateworld(point1);
            rotateworld(point2);
            //if (starHealth[i] > 0 && starHealth[i + 1] > 0) - Áûëî

            if (starHealth[starEdges[i][0]] > 0 && starHealth[starEdges[i][1]] > 0) // - Ñòàëî
            {

                float dx = point2.x - point1.x;
                float dy = point2.y - point1.y;
                float dz = point2.z - point1.z;
                float length = sqrt(dx * dx + dy * dy + dz * dz);
                int x = static_cast<int>(length) / 50;
                drawLine(point1, point2, x);// Ğèñîâàíèå çâ¸çäíûõ ëèíèé ñîçâåçäèÿ.
            }
        }
    }
    //void drawStarPulse(std::vector <point3d>& starArray, std::vector <float>& starHealth)

    void drawStarPulse(Constellation& Constellation)
    {
        std::vector <point3d>& starArray = Constellation.starsRenderedCords;
        std::vector <float>& starHealth = Constellation.starsHealth;

        int starsCount = starArray.size();
        HBRUSH brush = CreateSolidBrush(RGB(0, 191, 255));
        HBRUSH brush2 = CreateSolidBrush(RGB(255, 0, 0));
        SelectObject(window.context, brush);
        for (int i = 0; i < starsCount; i++)
        {
            point3d point;
            point.x = starArray[i].x;
            point.y = starArray[i].y;
            point.z = starArray[i].z;

            float a = timeGetTime() * .01;
            rotateworld(point);
            project(point);

            float dx = point.x - mouse.x;
            float dy = point.y - mouse.y;
            float lenght = sqrt(dx * dx + dy * dy);

            float rad = saturate(1.2 - lenght * .05) * fabs(sin(timeGetTime() * .01));

            SelectObject(window.context, brush);// Ïóëüñèğîâàíèå Çâ¸çä ïğè íàâåäåíèå ìûøè.

            if (GetAsyncKeyState(VK_LBUTTON))
            {
                if (lenght < starSize)
                {
                    SelectObject(window.context, brush2);
                    starHealth[i] -= .1;
                }
            }

            float sz = starSize * starHealth[i] + rad * 15;
            if (starHealth[i] > 0) //Àòàêà ïî ñîçâåçäèÿì óìåíüøàåò çâ¸çäû. 
            {
                drawPoint(point, sz);
            }

        }
        DeleteObject(brush);
        DeleteObject(brush2);
    }

    ///
    void drawHeroLinks(std::vector <point3d>& starArray, std::vector<std::vector<float>>& starEdges, std::vector <float>& starHealth)
    {

        int starsCount = starArray.size();
        for (int i = 0; i < starsCount - 1; i++)
        {
            point3d point1, point2;
            point1.x = starArray[starEdges[i][0]].x;
            point1.y = starArray[starEdges[i][0]].y;
            point1.z = starArray[starEdges[i][0]].z;

            point2.x = starArray[starEdges[i][1]].x;
            point2.y = starArray[starEdges[i][1]].y;
            point2.z = starArray[starEdges[i][1]].z;

            float a = timeGetTime() * .01;
            //rotateworld(point1);
            //rotateworld(point2);
            projectSingleConst(point1);
            projectSingleConst(point2);

            if (starHealth[i] > 0 && starHealth[i + 1] > 0)
            {

                float dx = point2.x - point1.x;
                float dy = point2.y - point1.y;
                float dz = point2.z - point1.z;
                float length = sqrt(dx * dx + dy * dy + dz * dz);
                int x = static_cast<int>(length*10) / 50;
                drawLine(point1, point2, x);// Ğèñîâàíèå çâ¸çäíûõ ëèíèé ñîçâåçäèÿ.
            }
        }
    }

    void drawHeroStarPulse(std::vector <point3d>& starArray, std::vector <float>& starHealth)
    {
        int starsCount = starArray.size();
        HBRUSH brush = CreateSolidBrush(RGB(0, 191, 255));
        HBRUSH brush2 = CreateSolidBrush(RGB(255, 0, 0));
        SelectObject(window.context, brush);
        for (int i = 0; i < starsCount; i++)
        {
            point3d point;
            point.x = starArray[i].x;
            point.y = starArray[i].y;
            point.z = starArray[i].z;

            float a = timeGetTime() * .01;
            rotateY(point, a);
            projectSingleConst(point);

            float dx = point.x - mouse.x;
            float dy = point.y - mouse.y;
            float lenght = sqrt(dx * dx + dy * dy);

            float rad = saturate(1.2 - lenght * .05) * fabs(sin(timeGetTime() * .01));

            SelectObject(window.context, brush);// Ïóëüñèğîâàíèå Çâ¸çä ïğè íàâåäåíèå ìûøè.

            if (GetAsyncKeyState(VK_LBUTTON))
            {
                if (lenght < starSize)
                {
                    SelectObject(window.context, brush2);
                    starHealth[i] -= .1;
                }
            }

            float sz = starSize * starHealth[i] + rad * 15;
            if (starHealth[i] > 0) //Àòàêà ïî ñîçâåçäèÿì óìåíüøàåò çâ¸çäû. 
            {
                drawPoint(point, sz);
            }

        }
        DeleteObject(brush);
        DeleteObject(brush2);
    }
    ///

    //void drawÑonstellation(std::vector <point3d>& starArray, std::vector<std::vector<float>>& starEdges, std::vector <float>& starHealth)
    void drawÑonstellation(Constellation& Constellation)
    {
        drawLinks(Constellation);
        drawStarPulse(Constellation);
    }

    void drawHeroÑonstellation(std::vector <point3d>& starArray, std::vector<std::vector<float>> &starEdges, std::vector <float>& starHealth)
    {
        drawHeroLinks(starArray, starEdges, starHealth);
        drawHeroStarPulse(starArray, starHealth);
    }

    void drawStarField()
    {
        srand(10);
        for (int i = 0; i < 500; i++)
        {
            point3d point;
            genRandSphere(point);
            rotateworld(point);
            project(point);
            drawPoint(point);
            // Çâ¸çäû íà ôîíå èõ êîë-âî. è Êàäğ îñòàíîâêè.
        }
    }

    const COLORREF colors[] =
    {
        RGB(255, 0, 0),    // Êğàñíûé
        RGB(255, 165, 0),  // Îğàíæåâûé
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

            DeleteObject(brush);// Äàííûå äëÿ ğèñîâàíèå öâåò. êğóãà.
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
        
        //drawÑonstellation(morphArray, Morp_indices, Morp_health); Îòêëş÷åíî
    }
    void drawWorld()
    {
        drawBack();


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
                menuMonthprocessing();
                menuDayprocessing();
                menuConfirmationButton();
                break;

            case gameState_::Fight:
                drawStarField();
                drawÑonstellation(Aries);
                drawColorCircle();
                std::string curentSignstring = zodiacSignToString(player_sign);
                TextOutA(window.context, window.width * 5 / 6, 0, curentSignstring.c_str(), curentSignstring.size());
                break;

        }

        drawGameCursor();

        /*if (confirm)
            {
                
                //drawÑonstellation(Aries);
                //drawÑonstellation(UrsaMajorCopy, UrsaMajor_indices, UrsaMajor_health);
                //drawÑonstellation(CancerCopy, Cancer_indices, Cancer_health);
                //drawÑonstellation(TaurusCopy, Taurus_indices, Taurus_health);
                //drawÑonstellation(LeoCopy, Leo_indices, Leo_health);
                //drawÑonstellation(GeminiCopy, Gemini_indices, Gemini_health);

                //drawÑonstellation(LibraCopy, Libra_indices, Libra_health);
                //drawHeroÑonstellation(LibraHeroCopy, Libra_indices, Libra_health);
                //drawÑonstellation(Libra, Libra_indices, Libra_health);

                //drawÑonstellation(VirgoCopy, Virgo_indices, Virgo_health);
                //drawÑonstellation(ScorpiusCopy, Scorpius_indices, Scorpius_health);
                //drawÑonstellation(SagittariusCopy, Sagittarius_indices, Sagittarius_health);
                //drawÑonstellation(CapricornusCopy, Capricornus_indices, Capricornus_health);
                //drawÑonstellation(AquariusCopy,Aquarius_indices, Aquarius_health);
                //drawÑonstellation(PiscesCopy, Pisces_indices, Pisces_health);

                //morphWepon(PiscesCopy, Pisces_indices, AquariusCopy, Aquarius_indices, MorphArray, Morp_indices, Morp_health); Îòêëş÷åíî

            }
            else
            {
                SelectDates();
                startTime = timeGetTime();
                int i = 0;
                //drawer::drawHeroÑonstellation(constStarArray[i], constIndArray[i], constHealthArray[i]); Îòêëş÷åíî
            }

        drawGameCursor();*/
    }
}

