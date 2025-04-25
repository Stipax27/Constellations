
int start;

bool isMoveActive = false; 


float finalS;

char whatKeyPressed() {
	if (GetAsyncKeyState('W') & 0x0001) {
		//TextOutA(window.context, window.width / 2 + 20, window.height / 2 + 50., "Работаем!", 20);

		return 'W';
	}
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

void fightMove(point3d& p) {

	
	//player_dodge_ofs ;

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
	//*SYM = { whatKeyPressed() };
		//if (!A || !S || !D) {
				//W = true;
		if (!isMoveActive)
		{
			//SYM[0] = 'A';
			SYM[0] = key;
			SYM[1] = '\0';


			start = timeGetTime();
			isMoveActive = true;
		}
		//}
	}

	if (isMoveActive) {
		T = timeGetTime() - start;
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
				
				//move(p, 0, currentS, 0);
			}
			else {
				currentA = F * (T / 1000 - moveTimer / 2);
				//F = 1;
				//currentA = -currentA;
				//move(p, 0, 2 * finalS - currentS, 0);
			}
			//
			char temp[64] = {0};
			//_ultoa_s(moveTimer - T / 1000, temp, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
			//_ultoa_s(currentA, temp, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
			//_ultoa_s(moveTimer - T / 1000, temp, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
			//TextOutA(window.context, window.width / 2, window.height / 2., (LPCSTR)temp, strlen(temp));
			/*SaveCurrentState();*/
		}
		else {
			isMoveActive = false;
		}



		
		/*SaveCurrentState();*/
	}
	
	move(p, player_dodge_ofs);
}

//

	/*
	if (isMoveActive) {

		T = timeGetTime() - start;

		if (T / 1000 < 3) {
			char temp[64] = { 0 };


			_ultoa_s(T / 1000, temp, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
			TextOutA(window.context, window.width / 2, window.height / 2., (LPCSTR)temp, strlen(temp));
			TextOutA(window.context, window.width / 2 + 20, window.height / 2., SYM, 1);

		}
		else {
			isMoveActive = false;
		}
	}
	*/
	

////////////////////////////


	//if (GetAsyncKeyState('A'))
	//{
	//	move(p, currentV, 0, 0);
	//};

	//if (GetAsyncKeyState('S'))
	//{
	//	move(p, 0, -currentV, 0);

	//};

	//if (GetAsyncKeyState('D'))
	//{
	//	move(p, -currentV, 0, 0);

	//};










	//int time = timeGetTime() - startTime;
	//std::string str = "hi" + time;
	//char temp[7];


	//_ultoa_s(time / 1000, temp, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt

	//TextOutA(window.context, window.width / 2, window.height / 2., (LPCSTR)temp, strlen(temp));

	//TextOutA(window.context, window.width / 2, window.height / 2., str.c_str(), str.size()); //Вывод текста














//void battleMove(point3d& b) {
//	char a[] = "qweqweqwe";
//	float moveStep = -100;
//
//		
//	if (GetAsyncKeyState('W'))
//	{
//		move(b, 0, moveStep, 0);
//	};
//	if (GetAsyncKeyState('A'))
//	{
//		move(b, moveStep, 0, 0);
//
//	};
//	if (GetAsyncKeyState('S'))
//	{
//		move(b, 0, -moveStep, 0);
//
//	};
//	if (GetAsyncKeyState('D'))
//	{
//		move(b, -moveStep, 0, 0);
//
//	};
//}
	//using namespace std::this_thread;     // sleep_for, sleep_until
	//using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
	//using std::chrono::system_clock;
//#include <chrono>
//#include <thread>
