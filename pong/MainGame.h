struct {
    int day, month;//���������� ��������� ����� � ���������� "������"
    bool action = false;//��������� - �������� (����� ������ ������ ������) ��� ����
} game;

void InitGame()
{
    initWorld();

    game.day = 32;
    game.month =  13;
    startTime = timeGetTime();
}

void MenuDrawer()
{
    HFONT hFont;
    bool fontInit = false;
    if (!fontInit)
    {
        hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
        fontInit = true;
    }

    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);
}



bool checkCollision(float textX, float textY, SIZE textSize)
{
    float x = textX - textSize.cx / 2;
    float y = textY - textSize.cy / 2;
    float x1 = textX + textSize.cx / 2;
    float y1 = textY + textSize.cy / 2;

    return (mouse.x > x && mouse.x < x1 && mouse.y > y && mouse.y < y1);
        
}

bool drawMenuItem(float x, float y, std::string& m)
{
    SIZE textSize;
    GetTextExtentPoint32(window.context, m.c_str(), m.size(), &textSize);
    bool over = checkCollision(x, y, textSize);

    SetTextColor(window.context, over ? RGB(0, 0, 255) : RGB(160, 160, 160));

    float x_ = x - textSize.cx / 2;
    float y_ = y - textSize.cy / 2;
    TextOutA(window.context, x_, y_, m.c_str(), m.size());

    return over;

}

void menuMonthprocessing()
{
    MenuDrawer();

    float circleRadius = 400;
    int numMonth = 12;
    
    for (int i = 0; i < numMonth; i++)
    {
        
        float angle = (2 * PI / numMonth) * i + timeGetTime() * 0.00015;

        float textX = window.width / 2 + circleRadius * cos(angle);
        float textY = window.height / 2 + circleRadius * sin(angle);
        
        std::string m = mounthToString((MonthSign)i);

        if (drawMenuItem(textX, textY,m))
        {
            if (GetAsyncKeyState(VK_LBUTTON))
            {
                player_month = (MonthSign)(i);
                gameState = gameState_::DaySelection;
                currentMonthIndex = i;
            }
        }
        else
        {
            currentMonthIndex = -1;
        }
    }

    
    std::string curentMounthstring = mounthToString(player_month);
    TextOutA(window.context, window.width * 5 / 8, 0, curentMounthstring.c_str(), curentMounthstring.size());

    SIZE textSize;
    std::string b = "select your date of birth";
    GetTextExtentPoint32(window.context, b.c_str(), b.size(), &textSize);
    TextOutA(window.context, window.width / 2 - textSize.cx / 2, 100, b.c_str(), b.size());
}
void menuDayprocessing()
{
    MenuDrawer();

    float circleRadius = 300;
    int numDay = 31;

    for (int i = 0;i < numDay;i++)
    {
        float angle = (2 * PI / numDay) * i - timeGetTime() * 0.00005;

        float textX = window.width / 2 + circleRadius * cos(angle);
        float textY = window.height / 2 + circleRadius * sin(angle);

        std::string m = std::to_string(i+1);
        SIZE textSize;
        GetTextExtentPoint32(window.context, m.c_str(), m.size(), &textSize);

        if (drawMenuItem(textX, textY, m))
        {
            if (GetAsyncKeyState(VK_LBUTTON))
            {
                player_day = i+1;
                currentDayIndex = i;
                gameState = gameState_::confirmSign;
            }
        }
        else
        {
            currentDayIndex = -1;
        }

        std::string curentDaystring = std::to_string(player_day);
        TextOutA(window.context, window.width * 5 / 8 + 100, 0, curentDaystring.c_str(), curentDaystring.size());
    }
}

void menuConfirmationButton()
{
    MenuDrawer();

    std::string m = "Play";
    SIZE textSize;
    GetTextExtentPoint32(window.context, m.c_str(), m.size(), &textSize);

    float textX = window.width / 2 - textSize.cx / 2;
    float textY = window.height / 1.2 - textSize.cy / 2;

    if (drawMenuItem(textX, textY, m))
    {
        SetTextColor(window.context, RGB(255, 0, 0));
        if (GetAsyncKeyState(VK_LBUTTON))
        {
            gameState = gameState_::Fight;
            startTime = timeGetTime();
        }
    }
    else
    {
        SetTextColor(window.context, RGB(160, 160, 160));
    }
}

void StartMenu()
{
    MenuDrawer();

    SIZE textSize;
    std::string m = "Play";
    GetTextExtentPoint32(window.context, m.c_str(), m.size(), &textSize);

    float textX = window.width / 2 - textSize.cx / 2;
    float textY = window.height / 2 - textSize.cy / 2;

    if (drawMenuItem(textX, textY, m))
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

    std::string b = "Quit";
    GetTextExtentPoint32(window.context, b.c_str(), b.size(), &textSize);

    float textX1 = window.width / 2 - textSize.cx / 2;
    float textY1 = window.height / 2 - textSize.cy / 2 + 100;

    if (drawMenuItem(textX1, textY1, b))
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
}

void Dialogue()
{
    //MenuDrawer();
    //
    //Dialog dialog("������ ���");
    //SIZE textSize;
    //GetTextExtentPoint32(window.context, d.c_str(), d.size(), &textSize);
    //drawMenuItem(window.width / 2, window.height / 4, d);

    /*std::string b = "I don't want meow!";
    GetTextExtentPoint32(window.context, b.c_str(), b.size(), &textSize);

    float textX = window.width / 2;
    float textY = window.height / 2 - textSize.cy / 2 + 50;

    if (drawMenuItem(textX, textY, b))
    {
        SetTextColor(window.context, RGB(255, 0, 0));
        if (GetAsyncKeyState(VK_LBUTTON))
        {
            gameState = gameState_::Fight;
            startTime = timeGetTime();
        }
    }
    else
    {
        SetTextColor(window.context, RGB(160, 160, 160));
    }
    std::string c = "I like this meow-story!";
    GetTextExtentPoint32(window.context, c.c_str(), c.size(), &textSize);

    float textX1 = window.width / 2;
    float textY1 = window.height / 2 - textSize.cy / 2 + 100 + textSize.cy;

    if (drawMenuItem(textX1, textY1, c))
    {
        SetTextColor(window.context, RGB(255, 0, 0));
        if (GetAsyncKeyState(VK_LBUTTON))
        {
            
        }
    }
    else
    {
        SetTextColor(window.context, RGB(160, 160, 160));
    }*/

}


