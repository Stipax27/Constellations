namespace drawer
{
    void rotateworld(point3d& p)
    {
        rotateX(p, mouseAngle.y * 0.1);
        rotateY(p, mouseAngle.x * 0.1);
        rotateZ(p, timeGetTime() * 0.01); // �������� ��������.
    }
    
    void drawScore()
    {
        //�������� �������� � �������
        SetTextColor(window.context, RGB(160, 160, 160));
        SetBkColor(window.context, RGB(0, 0, 0));
        SetBkMode(window.context, TRANSPARENT);
        auto hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
        auto hTmp = (HFONT)SelectObject(window.context, hFont);
        char txt[32];//����� ��� ������
        _itoa_s(game.score, txt, 10);//�������������� �������� ���������� � �����. ����� �������� � ���������� txt
        TextOutA(window.context, 10, 10, "Score", 5);
        TextOutA(window.context, 200, 10, (LPCSTR)txt, strlen(txt));
        _itoa_s(game.balls, txt, 10);
        TextOutA(window.context, 10, 100, "Balls", 5);
        TextOutA(window.context, 200, 100, (LPCSTR)txt, strlen(txt));
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
        rotateY(p, angleY); // ������������� ������������.
    }


    void drawPoint(point3d& p, float sz = 2)
    {
        if (p.z < 0) return;

        Ellipse(window.context,
            p.x - sz,
            p.y - sz,
            p.x + sz,
            p.y + sz
        );// ��������� ������. sz = ������ ������.

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
            // ��������� �����.
        }
    }

    void drawLinks(std::vector <float>& starArray, std::vector <float>& starHealth)
    {
        int starsCount = starArray.size() / 3;
        for (int i = 0; i < starsCount - 1; i++)
        {
            point3d point1, point2;
            point1.x = starArray[i * 3];
            point1.y = starArray[i * 3 + 1];
            point1.z = starArray[i * 3 + 2];

            point2.x = starArray[i * 3 + 3];
            point2.y = starArray[i * 3 + 3 + 1];
            point2.z = starArray[i * 3 + 3 + 2];

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
                drawLine(point1, point2, x);// ��������� ������� ����� ���������.
            }
        }
    }

    void drawStarPulse(std::vector <float>& starArray, std::vector <float>& starHealth)
    {
        int starsCount = starArray.size() / 3;
        HBRUSH brush = CreateSolidBrush(RGB(0, 191, 255));
        HBRUSH brush2 = CreateSolidBrush(RGB(255, 0, 0));
        SelectObject(window.context, brush);
        for (int i = 0; i < starsCount; i++)
        {
            point3d point;
            point.x = starArray[i * 3];
            point.y = starArray[i * 3 + 1];
            point.z = starArray[i * 3 + 2];

            float a = timeGetTime() * .01;
            rotateworld(point);
            project(point);

            float dx = point.x - mouse.x;
            float dy = point.y - mouse.y;
            float lenght = sqrt(dx * dx + dy * dy);

            float rad = saturate(1.2 - lenght * .05) * fabs(sin(timeGetTime() * .01));

            SelectObject(window.context, brush);// ������������� ���� ��� ��������� ����.

            if (GetAsyncKeyState(VK_LBUTTON))
            {
                if (lenght < starSize)
                {
                    SelectObject(window.context, brush2);
                    starHealth[i] -= .1;
                }
            }

            float sz = starSize * starHealth[i] + rad * 15;
            if (starHealth[i] > 0) //����� �� ���������� ��������� �����. 
            {
                drawPoint(point, sz);
            }

        }
        DeleteObject(brush);
        DeleteObject(brush2);
    }

    void draw�onstellation(std::vector <float>& starArray, std::vector <float>& starHealth)
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
            // ����� �� ���� �� ���-��. � ���� ���������.
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
        drawStarField();

        draw�onstellation(Aries, Aries_health);
        draw�onstellation(UrsaMajor, UrsaMajor_health);
        draw�onstellation(PieceOfCancer1, PieceOfCancer1_health);
        draw�onstellation(Taurus, Taurus_health);

        drawColorCircle();
        drawGameCursor();
    }
}