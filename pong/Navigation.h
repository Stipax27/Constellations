
int start;

bool isMoveActive = false; 

float finalS;

char whatKeyPressed() {
	if (GetAsyncKeyState('W') & 0x0001) 
		return 'W';
	if (GetAsyncKeyState('A') & 0x0001)
		return 'A';
	if (GetAsyncKeyState('S') & 0x0001)
		return 'S';
	if (GetAsyncKeyState('D') & 0x0001)
		return 'D';
	return '\0';  // Если ничего не нажато, возвращаем пустой символ
}

point3d player_dodge_ofs = { 0,0,0 };
point3d starfield_angles = { 0,0,0 };
point3d milkyway_angles = { 0,0,100 };


void fightMove(point3d& p) {

	float T;
	float moveTimer = 4;
	float currentV;
	float currentS;
	float currentA;
	float F = -1.5;
	bool isMoveStateBack; 

	static char SYM[2] = {0};

	char key = whatKeyPressed();

	if (!isMoveActive && key != '\0') // Проверка срабатывает если кнопка нажимается хотя бы один раз
	{
		if (!isMoveActive)
		{
			SYM[0] = key;
			SYM[1] = '\0';

			start = currentTime;
			isMoveActive = true;
		}
	}

	if (isMoveActive) {
		T = currentTime - start;
		currentA = F * (moveTimer / 2 - T / 1000);
		currentV = currentA * T / 1000;
		
		currentS = currentV * T / 1000;
		isMoveStateBack = false;
		if (T/1000 < moveTimer) {
			if (T/1000 > (moveTimer / 2))
			{
				isMoveStateBack = true;
			}
			if (!isMoveStateBack) {
				finalS = currentS;

				switch (SYM[0]) {
				case 'W':
					player_dodge_ofs = { 0, currentS, 0 };
					break;
				case 'A':
					player_dodge_ofs = { currentS, 0, 0 };
					break;
				case 'S':
					player_dodge_ofs = { 0, -currentS, 0 };
					break;
				case 'D':
					player_dodge_ofs = { -currentS, 0, 0 };
					break;

				
				}
			}
			else {
				currentA = F * (T / 1000 - moveTimer / 2);
				
			}
			char temp[64] = {0};
		}
		else {
			isMoveActive = false;
		}

	}
	
	p.move(p, player_dodge_ofs);
}
