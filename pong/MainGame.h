

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

    initWeapon();
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

                player_sign = getZodiacSign(player_day, player_month);
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
            gameState = gameState_::selectEnemy;
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
    //Dialog dialog("Привет мир");
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




void DrawStarsHP(HDC hdc) {
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);

    
    SetTextColor(hdc, RGB(100, 150, 255));
    for (size_t i = 0; i < starSet[player_sign]->starsRenderedCords.size(); ++i) {
        const auto& pos = starSet[player_sign]->starsRenderedCords[i];
        float hp = starSet[player_sign]->starsHealth[i];

        std::string hpText = "HP: " + std::to_string(static_cast<int>(hp));
        TextOutA(hdc, pos.x + 20, pos.y - 15, hpText.c_str(), hpText.size());
    }

    
    SetTextColor(hdc, RGB(255, 100, 100));
    for (size_t i = 0; i < starSet[currentEnemyID]->starsRenderedCords.size(); ++i) {
        const auto& pos = starSet[currentEnemyID]->starsRenderedCords[i];
        float hp = starSet[currentEnemyID]->starsHealth[i];

        std::string hpText = "HP: " + std::to_string(static_cast<int>(hp));
        TextOutA(hdc, pos.x + 20, pos.y - 15, hpText.c_str(), hpText.size());
    }
}

bool isBattleActive = false;
DWORD battleStartTime;
DWORD attackTime;

void StartBattle() {
    if (!isBattleActive) {
        battleStartTime = timeGetTime();
        attackTime = battleStartTime;
        isBattleActive = true;
        TextOutA(window.context, 400, 400, "Бой начался", 10);
    }
}

void UpdateGame() {
    static const DWORD MAX_BATTLE_TIME = 4 * 60 * 1000;
    static const DWORD MAX_REWIND = 30 * 1000;
    static DWORD battleTime = 2 * 60 * 1000;
    static DWORD timeModifier = 0;
    static DWORD lastInputTime = 0;
    const DWORD inputRepeatDelay = 100;


    DWORD currentTime = timeGetTime();



    if (GetAsyncKeyState('Q')) {
        if (currentTime - lastInputTime > inputRepeatDelay) {
            lastInputTime = currentTime;
            if (battleStartTime + battleTime + timeModifier + 1000 - currentTime <= MAX_BATTLE_TIME) {
                timeModifier += 1000;
            }
        }
    }
    else if (GetAsyncKeyState('E')) {
        if (currentTime - lastInputTime > inputRepeatDelay) {
            lastInputTime = currentTime;
            

            DWORD rewindAmount = 10000;
            DWORD targetTime = currentTime - rewindAmount;


            if (!battleHistory.empty() && targetTime < battleHistory.front().timestamp) {
                targetTime = battleHistory.front().timestamp;
            }

            if (RewindTime(targetTime)) {

                timeModifier = currentTime - targetTime;
            }
        }
    }

        if (isBattleActive) {
            LONG remainingTime = (LONG)((battleStartTime + battleTime + timeModifier) - currentTime);// Привязал оставшаеся время к лонгу

            DWORD totalBattleTime = battleTime + timeModifier;
            if (totalBattleTime > MAX_BATTLE_TIME) {
                timeModifier = MAX_BATTLE_TIME - battleTime;
                remainingTime = (LONG)((battleStartTime + MAX_BATTLE_TIME) - currentTime);
            }

            if (remainingTime > 0) {
                std::string timeStr = std::to_string(remainingTime / 1000);// Cтринг для вывода 
                TextOutA(window.context, 10, 10, "Время ", 6);
                TextOutA(window.context, 70, 10, timeStr.c_str(), timeStr.size());
            }
            else {
                timeModifier = 0;
                isBattleActive = false;
                gameState = gameState_::EndFight;
            }
        }
    
}




void endFight()
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

    std::string m = "You Lose";

    GetTextExtentPoint32(window.context, m.c_str(), m.size(), &textSize);

    if (mouse.x > window.width / 2 - textSize.cx / 2 && mouse.x < window.width / 2 + textSize.cx / 2 &&
        mouse.y > window.height / 2 - textSize.cy / 2 && mouse.y < window.height / 2 + textSize.cy / 2)
    {
        SetTextColor(window.context, RGB(255, 0, 0));
        if (GetAsyncKeyState(VK_LBUTTON))
        {
            gameState = gameState_::MainMenu;
        }
    }
    else
    {
        SetTextColor(window.context, RGB(160, 160, 160));
    }

    TextOutA(window.context, static_cast<int>(window.width / 2 - textSize.cx / 2), static_cast<int>(window.height / 2 - textSize.cy / 2), m.c_str(), m.size());
}
