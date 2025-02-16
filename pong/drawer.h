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

    void drawLinks(std::vector <std::vector <float>>& starArray, std::vector <float>& starHealth)
    {
        int starsCount = starArray.size();
        for (int i = 0; i < starsCount - 1; i++)
        {
            point3d point1, point2;
            point1.x = starArray[i][0];
            point1.y = starArray[i][1];
            point1.z = starArray[i][2];

            point2.x = starArray[i + 1][0];
            point2.y = starArray[i + 1][1];
            point2.z = starArray[i + 1][2];

            float a = timeGetTime() * .01;
            rotateworld(point1);
            rotateworld(point2);
            if (starHealth[i] > 0 && starHealth[i + 1] > 0)
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

    void drawStarPulse(std::vector <std::vector <float>>& starArray, std::vector <float>& starHealth)
    {
        int starsCount = starArray.size();
        HBRUSH brush = CreateSolidBrush(RGB(0, 191, 255));
        HBRUSH brush2 = CreateSolidBrush(RGB(255, 0, 0));
        SelectObject(window.context, brush);
        for (int i = 0; i < starsCount; i++)
        {
            point3d point;
            point.x = starArray[i][0];
            point.y = starArray[i][1];
            point.z = starArray[i][2];

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

    void drawÑonstellation(std::vector <std::vector <float>>& starArray, std::vector <float>& starHealth)
    {
        drawLinks(starArray, starHealth);
        drawStarPulse(starArray, starHealth);
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
        circleRadius = 75;
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

    void drawWorld()
    {
        drawBack();

        if (dayIsSelected && monthIsSelected)
        {
            drawStarField();

            drawÑonstellation(Aries, Aries_health);
            drawÑonstellation(UrsaMajor, UrsaMajor_health);
            drawÑonstellation(Cancer, Cancer_health);
            drawÑonstellation(Taurus, Taurus_health);
            drawÑonstellation(Leo, Leo_health);
            drawÑonstellation(Gemini, Gemini_health);
            drawÑonstellation(Libra, Libra_health);
            drawÑonstellation(Virgo, Virgo_health);
            drawÑonstellation(Scorpius, Scorpius_health);
            drawÑonstellation(Sagittarius, Sagittarius_health);
            drawÑonstellation(Capricornus, Capricornus_health);
            drawÑonstellation(Aquarius, Aquarius_health);
            drawÑonstellation(Pisces, Pisces_health);

            drawColorCircle();

            std::string curentSignstring = zodiacSignToString(player_sign);
            TextOutA(window.context, window.width * 5 / 6, 0, curentSignstring.c_str(), curentSignstring.size());
        }
        else
        {
            SelectDates();
            startTime = timeGetTime();
        }

        drawGameCursor();
    }
}

