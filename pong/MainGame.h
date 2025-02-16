struct {
    int day, month;//количество набранных очков и оставшихся "жизней"
    bool action = false;//состояние - ожидание (игрок должен нажать пробел) или игра
} game;

void InitGame()
{
    initWorld();

    game.day = 0, 32;
    game.month = 0, 13;
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
                  player_sign = (ZodiacSign)i;
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

    int menu_x = 100;
    int menu_y = 200;
    int menu_lenght = 100;
    for (int i = 0;i < 12;i++)
    {

        int item_height = 60;
        int menu_item_y = menu_y + i * item_height;

        if (menu_x < mouse.x && mouse.x < menu_x + menu_lenght &&
            mouse.y > menu_item_y && mouse.y < menu_item_y + item_height)
        {
            SetTextColor(window.context, RGB(100, 100, 100));
            if (GetAsyncKeyState(VK_LBUTTON))
            {
             player_month = (MonthSign)i;
            }
        }
        else
        {
            SetTextColor(window.context, RGB(160, 160, 160));
        }

        std::string m = mounthToString((MonthSign)i);
        TextOutA(window.context, menu_x, menu_y + i * item_height, m.c_str(), m.size());

        std::string curentMounthstring = mounthToString(player_month);
        TextOutA(window.context, window.width * 5 / 8, 0, curentMounthstring.c_str(), curentMounthstring.size());
    }
}