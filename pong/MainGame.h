struct {
    int score, balls;//���������� ��������� ����� � ���������� "������"
    bool action = false;//��������� - �������� (����� ������ ������ ������) ��� ����
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
//    cout << "������� ���� �������� (1-31): ";
//    cin >> day;
//    cout << "������� ����� �������� (1-12): ";
//    cin >> month;
//
//    try {
//        ZodiacSign sign = getZodiacSign(day, month);
//        cout << " ��� ���� ������� : " << zodiacSignToString(sign) << endl;
//    }
//    catch (const invalid_argument& e) {
//        cout << "������: " << e.what() << endl;
//    }
//
//    return 0;
//}