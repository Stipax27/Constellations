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