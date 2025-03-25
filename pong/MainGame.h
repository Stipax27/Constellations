struct {
    int day, month;//количество набранных очков и оставшихся "жизней"
    bool action = false;//состояние - ожидание (игрок должен нажать пробел) или игра
} game;

void InitGame()
{
    initWorld();

    game.day = 32;
    game.month =  13;
    startTime = timeGetTime();
}

HFONT hFont;
bool fontInit = false;

void menuMonthprocessing()
{
    if (!fontInit)
    {
        hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
        fontInit = true;
    }

    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);

    float circleRadius = 400;
    float centerX = window.width / 2;
    float centerY = window.height / 2;
    int numMonth = 12;
    SIZE textSize;

    for (int i = 0; i < numMonth; i++)
    {
        
        float angle = (2 * PI / numMonth) * i + timeGetTime() * 0.00015;

        
        float textX = centerX + circleRadius * cos(angle);
        float textY = centerY + circleRadius * sin(angle);

        
        std::string m = mounthToString((MonthSign)i);
        GetTextExtentPoint32(window.context, m.c_str(), m.size(), &textSize);

        int textWidth = textSize.cx;
        int textHeight = textSize.cy;

       
        if (mouse.x > textX - textWidth / 2 && mouse.x < textX + textWidth / 2 &&
            mouse.y > textY - textHeight / 2 && mouse.y < textY + textHeight / 2)
        {
            SetTextColor(window.context, RGB(0, 0, 255));
            if (GetAsyncKeyState(VK_LBUTTON))
            {
                player_month = (MonthSign)(i);
                gameState = gameState_::DaySelection;
                currentMonthIndex = i;
            }
        }
        else
        {
            SetTextColor(window.context, RGB(160, 160, 160));
            currentMonthIndex = -1;
        }

        
        TextOutA(window.context, static_cast<int>(textX - textWidth / 2), static_cast<int>(textY - textHeight / 2), m.c_str(), m.size());
    }

    
    std::string curentMounthstring = mounthToString(player_month);
    TextOutA(window.context, window.width * 5 / 8, 0, curentMounthstring.c_str(), curentMounthstring.size());

    std::string b = "select your date of birth";
    GetTextExtentPoint32(window.context, b.c_str(), b.size(), &textSize);
    TextOutA(window.context, window.width / 2 - textSize.cx / 2, 100, b.c_str(), b.size());
}
void menuDayprocessing()
{
    if (!fontInit)
    {
        hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
        fontInit = true;
    }

    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);

    float circleRadius = 300;
    float centerX = window.width / 2;
    float centerY = window.height / 2;
    int numDay = 31;

    for (int i = 0;i < numDay;i++)
    {

        float angle = (2 * PI / numDay) * i - timeGetTime() * 0.00005;

        float textX = centerX + circleRadius * cos(angle);
        float textY = centerY + circleRadius * sin(angle);

        std::string m = std::to_string(i+1);
        SIZE textSize;
        GetTextExtentPoint32(window.context, m.c_str(), m.size(), &textSize);


        int textWidth = textSize.cx;
        int textHeight = textSize.cy;

        if (mouse.x > textX - textWidth / 2 && mouse.x < textX + textWidth / 2 &&
            mouse.y > textY - textHeight / 2 && mouse.y < textY + textHeight / 2)
        {
            SetTextColor(window.context, RGB(255, 0, 0));
            if (GetAsyncKeyState(VK_LBUTTON))
            {
                player_day = i+1;
                currentDayIndex = i;
                gameState = gameState_::confirmSign;

                player_sign = getZodiacSign(player_day, player_month);
            }
        }
        else
        {
            SetTextColor(window.context, RGB(160, 160, 160));
            currentDayIndex = -1;
        }

        TextOutA(window.context, static_cast<int>(textX - textWidth / 2), static_cast<int>(textY - textHeight / 2), m.c_str(), m.size());

        std::string curentDaystring = std::to_string(player_day);
        TextOutA(window.context, window.width * 5 / 8 + 100, 0, curentDaystring.c_str(), curentDaystring.size());
    }
}

void menuConfirmationButton()
{
    if (!fontInit)
    {
        hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
        fontInit = true;
    }

    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);

    std::string m = "Play";
    SIZE textSize;
    GetTextExtentPoint32(window.context, m.c_str(), m.size(), &textSize);

    int textWidth = textSize.cx;
    int textHeight = textSize.cy;

    if (mouse.x > window.width / 2 - textWidth / 2 && mouse.x < window.width / 2 + textWidth / 2 &&
        mouse.y > window.height / 1.2 - textHeight / 2 && mouse.y < window.height / 1.2 + textHeight / 2)
    {
        SetTextColor(window.context, RGB(255, 0, 0));
        if (GetAsyncKeyState(VK_LBUTTON))
        {
            gameState = gameState_::selectEnemy;
            startTime = timeGetTime();
        }
    }
    else
    {
        SetTextColor(window.context, RGB(160, 160, 160));
    }

    TextOutA(window.context, static_cast<int>(window.width / 2 - textWidth / 2), static_cast<int>(window.height / 1.2 - textHeight / 2), m.c_str(), m.size());
}

void StartMenu()
{
    if (!fontInit)
    {
        hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
        fontInit = true;
    }

    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);

    SIZE textSize;

    std::string m = "Play";
   
    GetTextExtentPoint32(window.context, m.c_str(), m.size(), &textSize);

    if (mouse.x > window.width / 2 - textSize.cx / 2 && mouse.x < window.width / 2 + textSize.cx / 2 &&
        mouse.y > window.height / 2 - textSize.cy / 2 && mouse.y < window.height / 2 + textSize.cy / 2)
    {
        SetTextColor(window.context, RGB(255, 0, 0));
        if (GetAsyncKeyState(VK_LBUTTON))
        {
            gameState = gameState_::MonthSelection;
        }
    }
    else
    {
        SetTextColor(window.context, RGB(160, 160, 160));
    }

    TextOutA(window.context, static_cast<int>(window.width / 2 - textSize.cx / 2), static_cast<int>(window.height / 2 - textSize.cy / 2), m.c_str(), m.size());

    std::string b = "Quit";
    GetTextExtentPoint32(window.context, b.c_str(), b.size(), &textSize);

    if (mouse.x > window.width / 2 - textSize.cx / 2 && mouse.x < window.width / 2 + textSize.cx / 2 &&
        mouse.y > (window.height / 2) + 100 - textSize.cy / 2 && mouse.y < (window.height / 2) + 100 + textSize.cy / 2)
    {
        SetTextColor(window.context, RGB(255, 0, 0));
        if (GetAsyncKeyState(VK_LBUTTON))
        {
            ExitProcess(0);
        }
    }
    else
    {
        SetTextColor(window.context, RGB(160, 160, 160));
    }

    TextOutA(window.context, static_cast<int>(window.width / 2 - textSize.cx / 2), static_cast<int>((window.height / 2) +100 - textSize.cy / 2), b.c_str(), b.size());


}

/*HBRUSH brush;
HBRUSH brush2;
float finalStarRad = 0;
void drawLine22(point3d& p1, point3d& p2, Constellation& Constellation, int count)
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

        std::vector <float>& starHealth = Constellation.starsHealth;

        float dx = point.x - mouse.x;
        float dy = point.y - mouse.y;
        float lenght = sqrt(dx * dx + dy * dy);

        float rad = saturate(1.2 - lenght * .05) * fabs(sin(timeGetTime() * .01));

        if (dx > mouse.x - oldmouse.x and dy < mouse.y - oldmouse.y)
        {
            SelectObject(window.context, brush2);
            starHealth[i] -= .1;
        }

        else
        {
            SelectObject(window.context, brush);
        }

        finalStarRad = starSize * starHealth[i] + rad * 15;

    }



    //modelProject(point);

    //float sz = 1 + .5 * sinf(i + timeGetTime() * .01);
    //drawPoint(point, sz);
    // Рисование Линий.
}*/

void SelectVector()
{


    if (GetAsyncKeyState(VK_LBUTTON))
    {
        /*float dx = mouse.x - window.width / 2.;
        float dy = mouse.y - window.height / 2.;
        float lenght = sqrt(dx * dx + dy * dy);*/
        if (!lmb)
        {
            lmb = true;
            oldmouse.x = mouse.x;
            oldmouse.y = mouse.y;
            oldmouseAngle = mouseAngle;
        }
        float dx, dy;
        dx = mouse.x - oldmouse.x;
        dy = mouse.y - oldmouse.y;

        HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 191, 255));
        SelectObject(window.context, pen);
        MoveToEx(window.context, oldmouse.x, oldmouse.y, NULL);
        LineTo(window.context, mouse.x, mouse.y);

        //mouseAngle.x = oldmouseAngle.x + dx;
        //mouseAngle.y = oldmouseAngle.y + dy;
    }
    else
    {
    lmb = false;
    }

    //drawLine22();

}

/*void Attack()
{
    initWorld();
    
    if ()
    {

    

    }
    
}
*/
