struct {
    int score, balls;//количество набранных очков и оставшихся "жизней"
    bool action = false;//состояние - ожидание (игрок должен нажать пробел) или игра
} game;

void InitGame()
{
    initWorld();
    game.score = 0;
    game.balls = 9;
    startTime = timeGetTime();
}
   /* Aries =(21 March - 19 April);
    Taurus =(20 April - 20 May);
    Gemini =(21 May - 20 June);
    Cancer =(21 June - 22 July);
    Leo =(23 July - 22 August);
    Virgo =(23 August - 22 September);
    Libra =(23 September - 23 October);
    Scorpius =(24 October - 22 November);
    Sagittarius =(23 November - 21 December);
    Capricornus =(21 December - 19 January);
    Aquarius =(20 January - 19 February);
    Pisces =(20 February - 20 March);


void SelectDate()
{
    if (GetAsyncKeyState(VK_SPACE))
    {
        int userBirthData;
        if (userBirthData <= 365 && userBirthData > 0 )
        {
            if (userBirthData <= 50 && userBirthData > 30)
            {

            }
        }
   
}*/