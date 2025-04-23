bool isRewind = false;
DWORD attackTime;


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


bool drawClickableText(
    const std::string text,
    bool center,
    COLORREF COLOR_HOVER,
    int x = 0, int y = 0
) {

    COLORREF COLOR_REGULAR = RGB(160, 160, 160);

    point3d sz = drawString(text.c_str(), x, y, 1.f, true,true);

    bool isHovered = (mouse.x > x-sz.x/2 && mouse.x < x + sz.x/2 &&
        mouse.y > y && mouse.y < y + sz.y);

    textStyle.color= isHovered ? COLOR_HOVER : COLOR_REGULAR;


    drawString(text.c_str(), x, y, 1.f, true);


    //TextOutA(window.context, textX, textY, text.c_str(), text.size());

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

    drawCircularMenu(400, 0.00015, mounthString, 12, first);

    drawString(mounthToString(player_month).c_str(), window.width * 5 / 8, 0,1,true);
    
    drawString("select your date of birth", window.width / 2, 100, 1, true);
    
}


void menuDayprocessing()
{
    

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
    

    if (drawClickableText("Play", false, RGB(0, 0, 255), window.width / 2, window.height / 1.2))
    {
        gameState = gameState_::selectEnemy;
        startTime = timeGetTime();
    }
}

void StartMenu()
{
    if (drawClickableText("Play", true, RGB(0, 191, 255),window.width/2,window.height/2))
    {
        gameState = gameState_::MonthSelection;
    }

    if (drawClickableText("Quit", false, RGB(0, 191, 255), window.width / 2, window.height / 2 + 100))
    {
        ExitProcess(0);
    }

}

point3d winMessagePos;
DWORD winFightStartTime;

bool winEnter = false;

void calcWinMessageCoords()
{
    int amp = window.width / 2;

    winMessagePos.x = window.width / 2 + rand() % amp- amp/2;
    winMessagePos.y = window.height / 2 + rand() % amp - amp / 2;
    winMessagePos.z = 0;

}

void winFight() 
{
    if (!winEnter)
    {
        winFightStartTime = currentTime;
        winEnter = true;
        calcWinMessageCoords();
    }



    if (currentTime > winFightStartTime + 1000)
    {
        calcWinMessageCoords();
        winFightStartTime = currentTime;
    }

    std::string w = ("You WIN !!!!");
    drawString(w.c_str(), winMessagePos.x, winMessagePos.y, 1, true);
    

    if (GetAsyncKeyState(VK_LBUTTON))
    {
        gameState = gameState_::MainMenu;
    }

}

void endFight()
{
    if (drawClickableText("You Lose (((", true, RGB(255, 0, 0)))
    {
        gameState = gameState_::MainMenu;
    }
}

//void Shaking() 
//{
//    if ()
//    {
//    
//    }
//}
