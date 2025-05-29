#include <vector>
#include "ConverterSVGto3D.h"

bool isRewind = false;
DWORD attackTime;

struct {
    int day, month;
    bool action = false;
    std::vector<point3d> skeletonVertices;
    std::vector<std::pair<int, int>> skeletonEdges;
    float skeletonScale = 1.0f;
    float boneRotation = 0.0f;
} 
game;

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

    ConverterSVGto3D::Convert("assets/character.svg", //SVG файл для конвертации кидать сюда, позже нужно будет развить эту историю для добавления большего количества сущностей в игре TODO
        game.skeletonVertices,
        game.skeletonEdges);

    initWeapon();
}


bool drawClickableText(
    const std::string text,
    bool center,
    COLORREF COLOR_HOVER,
    int x = 0, int y = 0,
    float scale = 1.f) 
{

    COLORREF COLOR_REGULAR = RGB(160, 160, 160);

    point3d sz = drawString(text.c_str(), x, y, scale, true,true);

    bool isHovered = (mouse.x > x-sz.x/2 && mouse.x < x + sz.x/2 &&
        mouse.y > y && mouse.y < y + sz.y);

    textStyle.color= isHovered ? COLOR_HOVER : COLOR_REGULAR;


    drawString(text.c_str(), x, y, scale, true);


    //TextOutA(window.context, textX, textY, text.c_str(), text.size());

    return isHovered && GetAsyncKeyState(VK_LBUTTON);
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

    //drawCircularMenu(450, 0.00001, mounthString, 12, first);
    drawCircularMenu(450, 0.0001, mounthString, 12, first);

    drawString(mounthToString(player_month).c_str(), window.width *.1, window.height/2.,1,true);
    
    drawString("select your date of birth.", window.width / 2, window.height*.05, 1, true);
    
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

    drawString(days[player_day-1].c_str(), window.width / 2 + 800, window.height / 2., 1, true);// Вывод 1го числа 
}

void menuConfirmationButton()
{
   

    if (drawClickableText("Play", false, RGB(0, 191, 255), window.width / 2, window.height - window.height*.1,2.5))
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

    winMessagePos.x = window.width / 2 + rand() % amp- amp/2;
    winMessagePos.y = window.height / 2 + rand() % amp - amp / 2;
    winMessagePos.z = 0;

}

point3d exitButtonPos = { 0, 0, 0 }; 
bool wasExitHovered = false;
bool exitButtonInitialized = false;

void StartMenu()
{
    
    if (!exitButtonInitialized) {
        exitButtonPos.x = window.width / 2;
        exitButtonPos.y = window.height / 2 + 100;
        exitButtonInitialized = true;
    }

    
    if (drawClickableText("Play", true, RGB(0, 191, 255), window.width / 2, window.height / 2))
    {
        gameState = gameState_::MonthSelection;
    }

    std::string EXIT = "Quit ((";

    
    point3d textSize = drawString(EXIT.c_str(), exitButtonPos.x, exitButtonPos.y, 1.f, true, true);

    
    bool isHoveredNow = (mouse.x > exitButtonPos.x - textSize.x / 2 &&
        mouse.x < exitButtonPos.x + textSize.x / 2 &&
        mouse.y > exitButtonPos.y - textSize.y / 2 &&
        mouse.y < exitButtonPos.y + textSize.y / 2);

    
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
    for (int i =0;i<12;i++) 
    {
        auto& currentConst = starSet[i];

        for (int j = 0; j< currentConst->starsHealth.size(); j++)
        {
            currentConst->starsHealth[j] = 1;
        }
    }
    if (drawClickableText("You Lose (((", true, RGB(0, 191, 255),window.width/2,window.height/2))
    {
        gameState = gameState_::MainMenu;
    }
}

void draw3DLine(const point3d& start, const point3d& end, COLORREF color) {
    // Проекция с учетом перспективы
    float depthScale = 1.0f / (1.0f + start.z * 0.01f);
    int x1 = static_cast<int> (window.width / 2 + (start.x - window.width / 2) * depthScale);
    int y1 = static_cast<int> (window.height / 2 + (start.y - window.height / 2) * depthScale);

    depthScale = 1.0f / (1.0f + end.z * 0.01f);
    int x2 = static_cast<int> (window.width / 2 + (end.x - window.width / 2) * depthScale);
    int y2 = static_cast<int> (window.height / 2 + (end.y - window.height / 2) * depthScale);

    // Рисуем линию через GDI (да да винапи)
    HPEN hPen = CreatePen(PS_SOLID, 2, color);
    HPEN hOldPen = (HPEN)SelectObject(window.context, hPen);

    MoveToEx(window.context, x1, y1, NULL);
    LineTo(window.context, x2, y2);

    SelectObject(window.context, hOldPen);
    DeleteObject(hPen);
}

void RenderSkeleton() {
    for (auto& edge : game.skeletonEdges) {
        point3d start = {
            game.skeletonVertices[edge.first].x * game.skeletonScale,
            game.skeletonVertices[edge.first].y * game.skeletonScale,
            0
        };

        point3d end = {
            game.skeletonVertices[edge.second].x * game.skeletonScale,
            game.skeletonVertices[edge.second].y * game.skeletonScale,
            0
        };
        draw3DLine(start, end, RGB(255, 0, 0));
    }
}

void TransformCharacter() {
    game.skeletonScale = 1.0f + 0.2f * sin(currentTime * 0.001f);
    game.boneRotation = currentTime * 0.0005f;
}

