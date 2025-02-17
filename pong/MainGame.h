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

void ShowTXT()
{
    if (!fontInit)
    {
        hFont = CreateFont(70, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, "CALIBRI");
        fontInit = true;
    }

    SetTextColor(window.context, RGB(160, 160, 160));
    SetBkColor(window.context, RGB(0, 0, 0));
    SetBkMode(window.context, TRANSPARENT);

    int menu_x = 10;
    int menu_y = 100;
    int menu_lenght = 100;
    for (int i = 0;i < 12;i++)
    {
    
            int item_height = 40;
            int menu_item_y = menu_y + i * item_height;

          if (menu_x < mouse.x && mouse.x < menu_x+menu_lenght &&
              mouse.y > menu_item_y && mouse.y < menu_item_y + item_height)
          {
              SetTextColor(window.context, RGB(100, 100, 100));
              if (GetAsyncKeyState(VK_LBUTTON))
              {
                  player_sign = (ZodiacSign)(i+1);
              }
          }
          else
          {
              SetTextColor(window.context, RGB(160, 160, 160));
          }
      
        std::string m = zodiacSignToString((ZodiacSign)i);
        TextOutA(window.context, menu_x, menu_y +i* item_height, m.c_str(), m.size());

        std::string curentSignstring = zodiacSignToString(player_sign);
        TextOutA(window.context, window.width * 5 / 6, 0, curentSignstring.c_str(), curentSignstring.size());
    }
}

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

    for (int i = 0; i < numMonth; i++)
    {
        
        float angle = (2 * PI / numMonth) * i;

        
        float textX = centerX + circleRadius * cos(angle);
        float textY = centerY + circleRadius * sin(angle);

        
        std::string m = mounthToString((MonthSign)i);
        SIZE textSize;
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
                monthIsSelected = true;
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

        float angle = (2 * PI / numDay) * i;

        float textX = centerX + circleRadius * cos(angle);
        float textY = centerY + circleRadius * sin(angle);

        std::string m = dayToString((DaySign)(i));
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
                player_day = (DaySign)(i);
                dayIsSelected = true;
                currentDayIndex = i;
            }
        }
        else
        {
            SetTextColor(window.context, RGB(160, 160, 160));
            currentDayIndex = -1;
        }

        TextOutA(window.context, static_cast<int>(textX - textWidth / 2), static_cast<int>(textY - textHeight / 2), m.c_str(), m.size());

        std::string curentDaystring = dayToString(player_day);
        TextOutA(window.context, window.width * 5 / 8 + 100, 0, curentDaystring.c_str(), curentDaystring.size());
    }
}
void SelectDates()
{

    menuMonthprocessing();

    if (monthIsSelected)
    {
        menuDayprocessing();
    }


    if (dayIsSelected && monthIsSelected)
    {
        player_sign = getZodiacSign(player_day, player_month);
    }

}