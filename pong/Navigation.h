int start;
bool isMoveActive = false;

void fightMove(point3d& p) {

	//int time = timeGetTime() - startTime;
	//std::string str = "hi" + time;
	//char temp[7];


	//_ultoa_s(time / 1000, temp, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt

	//TextOutA(window.context, window.width / 2, window.height / 2., (LPCSTR)temp, strlen(temp));

	//TextOutA(window.context, window.width / 2, window.height / 2., str.c_str(), str.size()); //Вывод текста


	float moveStep = -1;

	//if (isWPressed) {
	if (GetAsyncKeyState('W') & 0x0001) // Проверка срабатывает если кнопка нажимается хотя бы один раз
	{
		if (!isMoveActive)
		{
			start = timeGetTime();
			isMoveActive = true;
		}
	}

	if (isMoveActive) {

		int time = (timeGetTime() - start);
		if (time/1000 < 6) {

			float currentMove = moveStep * time / 5000;

			move(p, 0, currentMove, 0);


			//

			char temp[7];
			_ultoa_s(time / 1000, temp, 10);//преобразование числовой переменной в текст. текст окажется в переменной txt
			TextOutA(window.context, window.width / 2, window.height / 2., (LPCSTR)temp, strlen(temp));

		}
		else {
			isMoveActive = false;
		}
	}
	



////////////////////////////




	if (GetAsyncKeyState('A'))
	{
		move(p, moveStep, 0, 0);
	};

	if (GetAsyncKeyState('S'))
	{
		move(p, 0, -moveStep, 0);

	};

	if (GetAsyncKeyState('D'))
	{
		move(p, -moveStep, 0, 0);

	};
}























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
