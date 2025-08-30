bool isRewind = false;

struct {
    int day, month;//?????????? ????????? ????? ? ?????????? "??????"
    bool action = false;//????????? - ???????? (????? ?????? ?????? ??????) ??? ????
} game;

enum menu_type {
    first,
    second
};

void InitGame()
{
    initWorld();
    initWeapon();
    initElements();

    game.day = 32;
    game.month = 13;
    startTime = currentTime;

    mciSendString(TEXT("play ..\\dx11minimal\\Resourses\\Sounds\\GG_C.mp3"), NULL, 0, NULL);
}

bool drawClickableText(
    const std::string& text,
    bool center,
    COLORREF COLOR_HOVER,
    int x = 0, int y = 0,
    float scale = 1.f)
{
    static std::string lastHoveredText; // Запоминаем последний наведенный текст
    COLORREF COLOR_REGULAR = RGB(160, 160, 160);

    // Первый проход - получаем размеры текста
    point3d sz = drawString(text.c_str(), x, y, scale, true, true);

    // Проверяем наведение мыши
    bool isHovered;
    if (center) {
        isHovered = (mouse.pos.x > x - sz.x / 2 && mouse.pos.x < x + sz.x / 2 &&
            mouse.pos.y > y && mouse.pos.y < y + sz.y);
    }
    else {
        isHovered = (mouse.pos.x > x && mouse.pos.x < x + sz.x &&
            mouse.pos.y > y && mouse.pos.y < y + sz.y);
    }

    // Проигрываем звук только при первом наведении на этот текст
    if (isHovered && lastHoveredText != text) {
        ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\Mouse_select.wav");
        lastHoveredText = text;
    }
    // Сбрасываем запомненный текст, если мышь не над ним
    else if (!isHovered && lastHoveredText == text) {
        lastHoveredText.clear();
    }

    // Устанавливаем цвет и рисуем текст
    textStyle.color = isHovered ? COLOR_HOVER : COLOR_REGULAR;
    drawString(text.c_str(), x, y, scale, center);

    return isHovered && (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
}

#include <unordered_set>
#include <functional>

bool drawParticledClickableText(
    const char* text,
    int x = 0, int y = 0,
    float scale = 1.f,
    bool centered = false
)
{
    static std::string lastHoveredText;
    static std::unordered_set<size_t> createdCoordinates;
 
    point3d sz = drawString(text, x, y, scale, false, true);

    bool isHovered;
    if (centered) {
        isHovered = (mouse.pos.x > x - sz.x / 2 && mouse.pos.x < x + sz.x / 2 &&
            mouse.pos.y > y && mouse.pos.y < y + sz.y);
    }
    else {
        isHovered = (mouse.pos.x > x && mouse.pos.x < x + sz.x &&
            mouse.pos.y > y && mouse.pos.y < y + sz.y);
    }

    if (isHovered && lastHoveredText != text) {
        ProcessSound("..\\dx11minimal\\Resourses\\Sounds\\Mouse_select.wav");
        lastHoveredText = text;
    }
    else if (!isHovered && lastHoveredText == text) {
        lastHoveredText.clear();
    }
  
    bool allMatch = false;

    for (int i = 0; i < renderTextList.size(); i++)
    {
        // Если нашли совпадение
        if (renderTextList[i]->str == text &&
            renderTextList[i]->x == x &&
            renderTextList[i]->y == y)
        {
            allMatch = true;
            break;
        }
    }

    // Создаем только если НЕТ совпадения
    if (!allMatch) {
        CreateParticledText(text, x, y, scale, centered);
    }

    return isHovered && (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
}

void drawCircularMenu(float circleRadius, float speed, string* items, int size, menu_type selectedType) {

    circleRadius *= window.width / 2560.;
    float centerX = window.width / 2;
    float centerY = window.height / 2;
    for (int i = 0; i < size; i++)
    {
        float angle = (2 * PI / size) * i + currentTime * speed;
        float textX = centerX + circleRadius * cos(angle);
        float textY = centerY + circleRadius * sin(angle);

        bool isClicked = drawClickableText(items[i], false, RGB(0, 191, 255), textX, textY);
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

            break;
        }
    }

}

//

void menuMonthprocessing()
{
    drawCircularMenu(450, 0.0001, mounthString, 12, first);

    drawString(mounthToString(player_month).c_str(), window.width * .1, window.height / 2., 1, true);

    drawString("select your date of birth.", window.width / 2, window.height * .05, 1, true);
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

    drawCircularMenu(300, 0.00002, days, 31, second);

    drawString(days[player_day - 1].c_str(), window.width / 2 + 800, window.height / 2., 1, true);// ????? 1?? ????? 
}

void menuConfirmationButton()
{
    if (drawClickableText("Play", false, RGB(0, 191, 255), window.width / 2, window.height - window.height * .1, 2.5))
    {
        gameState = gameState_::DialogStruct;

    }
}

point3d winMessagePos;
DWORD winFightStartTime;

bool winEnter = false;

void calcWinMessageCoords()
{
    int amp = window.width / 2;

    winMessagePos.x = window.width / 2 + rand() % amp - amp / 2;
    winMessagePos.y = window.height / 2 + rand() % amp - amp / 2;
    winMessagePos.z = 0;
}

point3d exitButtonPos = { 0, 0, 0 };
bool wasExitHovered = false;
bool exitButtonInitialized = false;

void TestDrawLine()
{

    Draw::NullDrawer(1, 1);
}

bool textBool = false;

void StartMenu()
{
    if (!exitButtonInitialized) {
        exitButtonPos.x = window.width / 2;
        exitButtonPos.y = window.height / 2 + 100;
        exitButtonInitialized = true;
    }

    const char* text = "Play";
    const char* text1 = "Settings";
    const char* text2 = "Authors";

    if (drawParticledClickableText(text, window.width / 2, window.height / 2, 1.f, true))
    {
        DeleteParticledText(text);
        DeleteParticledText(text1);
        DeleteParticledText(text2);

        gameState = gameState_::DialogStruct;
    }

    if (drawParticledClickableText(text1, window.width / 2, window.height / 2 + 200, 1.f, true))
    {
        DeleteParticledText(text);
        DeleteParticledText(text1);
        DeleteParticledText(text2);

        gameState = gameState_::Settings;
    }

    if (drawParticledClickableText(text2, window.width / 2, window.height / 2 + 300, 1.f, true))
    {
        DeleteParticledText(text);
        DeleteParticledText(text1);
        DeleteParticledText(text2);
        gameState = gameState_::Authors;
    }

    //if (drawClickableText("Play", true, RGB(0, 191, 255), window.width / 2, window.height / 2))
    //{
    //    gameState = gameState_::MonthSelection;
    //}

    std::string EXIT = "Quit ((";

    point3d textSize = drawString(EXIT.c_str(), exitButtonPos.x, exitButtonPos.y, 1.f, true, true);

    bool isHoveredNow = (mouse.pos.x > exitButtonPos.x - textSize.x / 2 &&
        mouse.pos.x < exitButtonPos.x + textSize.x / 2 &&
        mouse.pos.y > exitButtonPos.y - textSize.y / 2 &&
        mouse.pos.y < exitButtonPos.y + textSize.y / 2);

    if (isHoveredNow && !wasExitHovered)
    {
        exitButtonPos.x = 100 + rand() % (window.width - 200);
        exitButtonPos.y = 100 + rand() % (window.height - 200);
    }
    wasExitHovered = isHoveredNow;

    if (drawClickableText(EXIT, true, RGB(0, 191, 255), exitButtonPos.x, exitButtonPos.y))
    {
        ExitProcess(0);
    }


}

void SettingsState()
{
    const char* text = "Back";
    if (drawParticledClickableText(text, window.width / 2, window.height / 2 - 100, 1.f, true))
    {
        DeleteParticledText(text);
        gameState = gameState_::MainMenu;
    }
}

void AuthorsState()
{
    const char* text = "Back";
    if (drawParticledClickableText(text, window.width / 2, window.height / 2 - 100, 1.f, true))
    {
        DeleteParticledText(text);
        gameState = gameState_::MainMenu;
    }
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
    for (int i = 0;i < 12;i++)
    {
        auto& currentConst = starSet[i];

        for (int j = 0; j < currentConst->starsHealth.size(); j++)
        {
            currentConst->starsHealth[j] = 1;
        }
    }
    if (drawClickableText("You Lose (((", true, RGB(0, 191, 255), window.width / 2, window.height / 2))
    {
        gameState = gameState_::MainMenu;
    }
}

