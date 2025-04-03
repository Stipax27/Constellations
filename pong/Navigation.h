
int start;
bool isMoveActive = false;

float finalS;
			

void fightMove(point3d& p) {


	float T;
	float moveTimer = 4;
	float currentV;
	float currentS;
	float currentA;
	float F = -2.5;
	bool isMoveStateBack;

	if (GetAsyncKeyState('W') & 0x0001) // Проверка срабатывает если кнопка нажимается хотя бы один раз
	{
		if (!isMoveActive)
		{
			start = timeGetTime();
			isMoveActive = true;
		}
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
				move(p, 0, currentS, 0);
				finalS = currentS;
			}
			else {
				currentA = F * (T / 1000 - moveTimer / 2);

				//F = 1;
				//currentA = -currentA;

				//move(p, 0, 2 * finalS - currentS, 0);
			}


			//
			char temp[7] = {0};
			//_ultoa_s(moveTimer - T / 1000, temp, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt

			//_ultoa_s(currentA / 1000, temp, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt

			_ultoa_s(moveTimer - T / 1000, temp, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
			TextOutA(window.context, window.width / 2, window.height / 2., (LPCSTR)temp, strlen(temp));

		}
		else {
			isMoveActive = false;
		}
	}
	



////////////////////////////




	if (GetAsyncKeyState('A'))
	{
		move(p, currentV, 0, 0);
	};

	if (GetAsyncKeyState('S'))
	{
		move(p, 0, -currentV, 0);

	};

	if (GetAsyncKeyState('D'))
	{
		move(p, -currentV, 0, 0);

	};
}









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
