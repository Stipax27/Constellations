struct {
    int score, balls;//количество набранных очков и оставшихс€ "жизней"
    bool action = false;//состо€ние - ожидание (игрок должен нажать пробел) или игра
} game;

void InitGame()
{
    initWorld();
    game.score = 0;
    game.balls = 9;

    startTime = timeGetTime();
}
//int requestDate() {
//    int day, month;
//
//    cout << "¬ведите день рождени€ (1-31): ";
//    cin >> day;
//    cout << "¬ведите мес€ц рождени€ (1-12): ";
//    cin >> month;
//
//    try {
//        ZodiacSign sign = getZodiacSign(day, month);
//        cout << " ¬аш знак зодиака : " << zodiacSignToString(sign) << endl;
//    }
//    catch (const invalid_argument& e) {
//        cout << "ќшибка: " << e.what() << endl;
//    }
//
//    return 0;
//}