

struct {
    int day, month;//количество набранных очков и оставшихся "жизней"
    bool action = false;//состояние - ожидание (игрок должен нажать пробел) или игра
} game;

enum menu_type {
    first,
    second
};

void InitGame()
{
    initWorld();

    game.day = 32;
    game.month = 13;
    startTime = timeGetTime();

    initWeapon();
}

HFONT hFont;
bool isFontInit = false;

// UI utils
void fontInit(bool& isInitialized) {
    if (!isInitialized)
    {
        hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
        isInitialized = true;
    }
    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 255));
    SetBkMode(window.context, TRANSPARENT);
}


bool drawClickableText(
    const std::string text,
    bool center,
    COLORREF COLOR_HOVER,
    int x = 0, int y = 0
) {

    COLORREF COLOR_REGULAR = RGB(160, 160, 160);

    SIZE textSize;
    GetTextExtentPoint32(window.context, text.c_str(), text.size(), &textSize);

    int textX = static_cast<int>(center ? (window.width / 2) - (textSize.cx / 2) : x - (textSize.cx / 2));
    int textY = static_cast<int>(center ? (window.height / 2) - (textSize.cy / 2) : y - (textSize.cy / 2));

    bool isHovered = (mouse.x > textX && mouse.x < textX + textSize.cx &&
        mouse.y > textY && mouse.y < textY + textSize.cy);

    SetTextColor(window.context, isHovered ? COLOR_HOVER : COLOR_REGULAR);

    TextOutA(window.context, textX, textY, text.c_str(), text.size());

    return isHovered && GetAsyncKeyState(VK_LBUTTON);
}

void drawCircularMenu(float circleRadius, float speed, string* items, int size, menu_type selectedType) {


    float centerX = window.width / 2;
    float centerY = window.height / 2;
    for (int i = 0; i < size; i++)
    {
        float angle = (2 * PI / size) * i + timeGetTime() * speed;
        float textX = centerX + circleRadius * cos(angle);
        float textY = centerY + circleRadius * sin(angle);

        bool isClicked = drawClickableText(items[i], false, RGB(0, 0, 255), textX, textY);
        switch (selectedType) {
        case first:
            if (isClicked) {
                player_month = (MonthSign)(i);
                gameState = gameState_::DaySelection;
                currentMonthIndex = i;
            }
            else
            {
                currentMonthIndex = -1;
            }
            break;

        case second:

            if (isClicked)
            {
                player_day = i + 1;
                currentDayIndex = i;
                gameState = gameState_::confirmSign;

                player_sign = getZodiacSign(player_day, player_month);
            }
            else
            {
                currentDayIndex = -1;
            }

            /*   TextOutA(window.context, static_cast<int>(textX - textWidth / 2), static_cast<int>(textY - textHeight / 2), m.c_str(), m.size());

               std::string curentDaystring = std::to_string(player_day);
               TextOutA(window.context, window.width * 5 / 8 + 100, 0, curentDaystring.c_str(), curentDaystring.size());*/
            break;
        }
    }

}

//

void menuMonthprocessing()
{
    fontInit(isFontInit);


    drawCircularMenu(400, 0.00015, mounthString, 12, first);

    SIZE textSize;

    std::string curentMounthstring = mounthToString(player_month);
    TextOutA(window.context, window.width * 5 / 8, 0, curentMounthstring.c_str(), curentMounthstring.size());

    std::string b = "select your date of birth";
    GetTextExtentPoint32(window.context, b.c_str(), b.size(), &textSize);
    TextOutA(window.context, window.width / 2 - textSize.cx / 2, 100, b.c_str(), b.size());
}


void menuDayprocessing()
{
    fontInit(isFontInit);

    float circleRadius = 300;
    float centerX = window.width / 2;
    float centerY = window.height / 2;
    int numDay = 31;


    std::string days[31];
    for (int i = 0; i < 31; i++)
    {
        days[i] = std::to_string(i + 1);
    }

    drawCircularMenu(300, 0.00005, days, 31, second);
}

void menuConfirmationButton()
{
    fontInit(isFontInit);

    if (drawClickableText("Play", false, RGB(0, 0, 255), window.width / 2, window.height / 1.2))
    {
        gameState = gameState_::selectEnemy;
        startTime = timeGetTime();
    }
}

void StartMenu()
{
    fontInit(isFontInit);

    if (drawClickableText("Play", true, RGB(255, 0, 0)))
    {
        gameState = gameState_::MonthSelection;
    }

    if (drawClickableText("Quit", false, RGB(255, 0, 0), window.width / 2, window.height / 2 + 100))
    {
        ExitProcess(0);
    }

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
    static DWORD battleTime = 2 * 5 * 1000;
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


            //if (//!battleHistory.empty() && targetTime < battleHistory.front().timestamp) {
            //    targetTime = battleHistory.front().timestamp;
            //}

            //if (//RewindOneStepBack()) {
            //    //fightMove(playerPosition, true);
            //    timeModifier = currentTime - targetTime;
            //}
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
    fontInit(isFontInit);

    if (drawClickableText("You Lose", true, RGB(255, 0, 0)))
    {
        gameState = gameState_::MainMenu;
    }
}


/*
void endFight()
{
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
*/

//void menuConfirmationButton()
//{
//    fontInit(isFontInit);
//
//    std::string m = "Play";
//    SIZE textSize;
//    GetTextExtentPoint32(window.context, m.c_str(), m.size(), &textSize);
//
//    int textWidth = textSize.cx;
//    int textHeight = textSize.cy;
//
//    if (mouse.x > window.width / 2 - textWidth / 2 && mouse.x < window.width / 2 + textWidth / 2 &&
//        mouse.y > window.height / 1.2 - textHeight / 2 && mouse.y < window.height / 1.2 + textHeight / 2)
//    {
//        SetTextColor(window.context, RGB(255, 0, 0));
//        if (GetAsyncKeyState(VK_LBUTTON))
//        {
//            gameState = gameState_::selectEnemy;
//            startTime = timeGetTime();
//        }
//    }
//    else
//    {
//        SetTextColor(window.context, RGB(160, 160, 160));
//    }
//
//    TextOutA(window.context, static_cast<int>(window.width / 2 - textWidth / 2), static_cast<int>(window.height / 1.2 - textHeight / 2), m.c_str(), m.size());
//}


//void StartMenu()
//{
//    fontInit(isFontInit);
//
//    SIZE textSize;
//
//    std::string m = "Play";
//
//    GetTextExtentPoint32(window.context, m.c_str(), m.size(), &textSize);
//
//    if (mouse.x > window.width / 2 - textSize.cx / 2 && mouse.x < window.width / 2 + textSize.cx / 2 &&
//        mouse.y > window.height / 2 - textSize.cy / 2 && mouse.y < window.height / 2 + textSize.cy / 2)
//    {
//        SetTextColor(window.context, RGB(255, 0, 0));
//        if (GetAsyncKeyState(VK_LBUTTON))
//        {
//            gameState = gameState_::MonthSelection;
//        }
//    }
//    else
//    {
//        SetTextColor(window.context, RGB(160, 160, 160));
//    }
//
//    TextOutA(window.context, static_cast<int>(window.width / 2 - textSize.cx / 2), static_cast<int>(window.height / 2 - textSize.cy / 2), m.c_str(), m.size());
//
//    std::string b = "Quit";
//    GetTextExtentPoint32(window.context, b.c_str(), b.size(), &textSize);
//
//    if (mouse.x > window.width / 2 - textSize.cx / 2 && mouse.x < window.width / 2 + textSize.cx / 2 &&
//        mouse.y >(window.height / 2) + 100 - textSize.cy / 2 && mouse.y < (window.height / 2) + 100 + textSize.cy / 2)
//    {
//        SetTextColor(window.context, RGB(255, 0, 0));
//        if (GetAsyncKeyState(VK_LBUTTON))
//        {
//            ExitProcess(0);
//        }
//    }
//    else
//    {
//        SetTextColor(window.context, RGB(160, 160, 160));
//    }
//
//    TextOutA(window.context, static_cast<int>(window.width / 2 - textSize.cx / 2), static_cast<int>((window.height / 2) + 100 - textSize.cy / 2), b.c_str(), b.size());
//
//
//}

//void menuMonthprocessing()
//{
//    fontInit(isFontInit);
//
//
//    float circleRadius = 400;
//    float centerX = window.width / 2;
//    float centerY = window.height / 2;
//    int numMonth = 12;
//    SIZE textSize;
//
//    for (int i = 0; i < numMonth; i++)
//    {
//
//        float angle = (2 * PI / numMonth) * i + timeGetTime() * 0.00015;
//
//
//        float textX = centerX + circleRadius * cos(angle);
//        float textY = centerY + circleRadius * sin(angle);
//
//
//        std::string m = mounthToString((MonthSign)i);
//        GetTextExtentPoint32(window.context, m.c_str(), m.size(), &textSize);
//
//        int textWidth = textSize.cx;
//        int textHeight = textSize.cy;
//
//
//        if (mouse.x > textX - textWidth / 2 && mouse.x < textX + textWidth / 2 &&
//            mouse.y > textY - textHeight / 2 && mouse.y < textY + textHeight / 2)
//        {
//            SetTextColor(window.context, RGB(0, 0, 255));
//            if (GetAsyncKeyState(VK_LBUTTON))
//            {
//                player_month = (MonthSign)(i);
//                gameState = gameState_::DaySelection;
//                currentMonthIndex = i;
//            }
//        }
//        else
//        {
//            SetTextColor(window.context, RGB(160, 160, 160));
//            currentMonthIndex = -1;
//        }
//
//
//        TextOutA(window.context, static_cast<int>(textX - textWidth / 2), static_cast<int>(textY - textHeight / 2), m.c_str(), m.size());
//    }
//
//
//    std::string curentMounthstring = mounthToString(player_month);
//    TextOutA(window.context, window.width * 5 / 8, 0, curentMounthstring.c_str(), curentMounthstring.size());
//
//    std::string b = "select your date of birth";
//    GetTextExtentPoint32(window.context, b.c_str(), b.size(), &textSize);
//    TextOutA(window.context, window.width / 2 - textSize.cx / 2, 100, b.c_str(), b.size());
//}


//void menuDayprocessing()
//{
//    fontInit(isFontInit);
//
//    float circleRadius = 300;
//    float centerX = window.width / 2;
//    float centerY = window.height / 2;
//    int numDay = 31;
//
//    for (int i = 0;i < numDay;i++)
//    {
//
//        float angle = (2 * PI / numDay) * i - timeGetTime() * 0.00005;
//
//        float textX = centerX + circleRadius * cos(angle);
//        float textY = centerY + circleRadius * sin(angle);
//
//        std::string m = std::to_string(i + 1);
//        SIZE textSize;
//        GetTextExtentPoint32(window.context, m.c_str(), m.size(), &textSize);
//
//
//        int textWidth = textSize.cx;
//        int textHeight = textSize.cy;
//
//        if (mouse.x > textX - textWidth / 2 && mouse.x < textX + textWidth / 2 &&
//            mouse.y > textY - textHeight / 2 && mouse.y < textY + textHeight / 2)
//        {
//            SetTextColor(window.context, RGB(255, 0, 0));
//            if (GetAsyncKeyState(VK_LBUTTON))
//            {
//                player_day = i + 1;
//                currentDayIndex = i;
//                gameState = gameState_::confirmSign;
//
//                player_sign = getZodiacSign(player_day, player_month);
//            }
//        }
//        else
//        {
//            SetTextColor(window.context, RGB(160, 160, 160));
//            currentDayIndex = -1;
//        }
//
//        TextOutA(window.context, static_cast<int>(textX - textWidth / 2), static_cast<int>(textY - textHeight / 2), m.c_str(), m.size());
//
//        std::string curentDaystring = std::to_string(player_day);
//        TextOutA(window.context, window.width * 5 / 8 + 100, 0, curentDaystring.c_str(), curentDaystring.size());
//    }
//}