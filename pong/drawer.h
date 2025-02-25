namespace drawer
{
    void rotateworld(point3d& p)
    {
        rotateX(p, mouseAngle.y * 0.1);
        rotateY(p, mouseAngle.x * 0.1);
        //rotateZ(p, timeGetTime() * 0.01); // Кручение обьектов.
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
        rotateY(p, angleY); // Сферообразное пространство.
    }


    void drawPoint(point3d& p, float sz = 2)
    {
        if (p.z < 0) return;

        Ellipse(window.context,
            p.x - sz,
            p.y - sz,
            p.x + sz,
            p.y + sz
        );// Рисование элипса. sz = Размер звезды.

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
            // Рисование Линий.
        }
    }

    void drawLinks(std::vector <point3d>& starArray, std::vector<std::vector<float>> starEdges, std::vector <float>& starHealth)
    {

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
            //if (starHealth[i] > 0 && starHealth[i + 1] > 0) - Было

            if (starHealth[starEdges[i][0]] > 0 && starHealth[starEdges[i][1]] > 0) // - Стало
            {

                float dx = point2.x - point1.x;
                float dy = point2.y - point1.y;
                float dz = point2.z - point1.z;
                float length = sqrt(dx * dx + dy * dy + dz * dz);
                int x = static_cast<int>(length) / 50;
                drawLine(point1, point2, x);// Рисование звёздных линий созвездия.
            }
        }
    }

    void drawStarPulse(std::vector <point3d>& starArray, std::vector <float>& starHealth)
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
            rotateworld(point);
            project(point);

            float dx = point.x - mouse.x;
            float dy = point.y - mouse.y;
            float lenght = sqrt(dx * dx + dy * dy);

            float rad = saturate(1.2 - lenght * .05) * fabs(sin(timeGetTime() * .01));

            SelectObject(window.context, brush);// Пульсирование Звёзд при наведение мыши.

            if (GetAsyncKeyState(VK_LBUTTON))
            {
                if (lenght < starSize)
                {
                    SelectObject(window.context, brush2);
                    starHealth[i] -= .1;
                }
            }

            float sz = starSize * starHealth[i] + rad * 15;
            if (starHealth[i] > 0) //Атака по созвездиям уменьшает звёзды. 
            {
                drawPoint(point, sz);
            }

        }
        DeleteObject(brush);
        DeleteObject(brush2);
    }

    ///
    void drawHeroLinks(std::vector <point3d>& starArray, std::vector<std::vector<float>> starEdges, std::vector <float>& starHealth)
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
            if (starHealth[i] > 0 && starHealth[i + 1] > 0)
            {

                float dx = point2.x - point1.x;
                float dy = point2.y - point1.y;
                float dz = point2.z - point1.z;
                float length = sqrt(dx * dx + dy * dy + dz * dz);
                int x = static_cast<int>(length) / 50;
                drawLine(point1, point2, x);// Рисование звёздных линий созвездия.
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
            //rotateworld(point);
            project(point);

            float dx = point.x - mouse.x;
            float dy = point.y - mouse.y;
            float lenght = sqrt(dx * dx + dy * dy);

            float rad = saturate(1.2 - lenght * .05) * fabs(sin(timeGetTime() * .01));

            SelectObject(window.context, brush);// Пульсирование Звёзд при наведение мыши.

            if (GetAsyncKeyState(VK_LBUTTON))
            {
                if (lenght < starSize)
                {
                    SelectObject(window.context, brush2);
                    starHealth[i] -= .1;
                }
            }

            float sz = starSize * starHealth[i] + rad * 15;
            if (starHealth[i] > 0) //Атака по созвездиям уменьшает звёзды. 
            {
                drawPoint(point, sz);
            }

        }
        DeleteObject(brush);
        DeleteObject(brush2);
    }
    ///

    void drawСonstellation(std::vector <point3d>& starArray, std::vector<std::vector<float>> starEdges, std::vector <float>& starHealth)
    {
        drawLinks(starArray, starEdges, starHealth);
        drawStarPulse(starArray, starHealth);
    }

    void drawHeroСonstellation(std::vector <point3d>& starArray, std::vector<std::vector<float>> starEdges, std::vector <float>& starHealth)
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
            // Звёзды на фоне их кол-во. и Кадр остановки.
        }
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

        drawСonstellation(AriesCopy, Aries_indices, Aries_health);
        //drawСonstellation(UrsaMajorCopy, UrsaMajor_indices, UrsaMajor_health);
        drawСonstellation(CancerCopy, Cancer_indices, Cancer_health);
        drawСonstellation(TaurusCopy, Taurus_indices, Taurus_health);
        drawСonstellation(LeoCopy, Leo_indices, Leo_health);
        drawСonstellation(GeminiCopy, Gemini_indices, Gemini_health);
        
        //drawСonstellation(LibraCopy, Libra_indices, Libra_health);
        //drawHeroСonstellation(LibraHeroCopy, Libra_indices, Libra_health);
        //drawСonstellation(Libra, Libra_indices, Libra_health);

        drawСonstellation(VirgoCopy, Virgo_indices, Virgo_health);
        drawСonstellation(ScorpiusCopy, Scorpius_indices, Scorpius_health);
        drawСonstellation(SagittariusCopy, Sagittarius_indices, Sagittarius_health);
        drawСonstellation(CapricornusCopy, Capricornus_indices, Capricornus_health);
        drawСonstellation(AquariusCopy,Aquarius_indices, Aquarius_health);
        drawСonstellation(PiscesCopy, Pisces_indices, Pisces_health);

        drawGameCursor();
    }
}

